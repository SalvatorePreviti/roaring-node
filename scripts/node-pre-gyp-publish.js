#!/usr/bin/env node

/*
Based on https://github.com/bchr02/node-pre-gyp-github

The MIT License (MIT)

Copyright (c) 2015 Bill Christo

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.

*/

const path = require("path");
const fs = require("fs");
const colors = require("chalk");

const { Octokit } = require("@octokit/rest");

const packageJson = require("../package.json");
const { execSync } = require("child_process");
const { runMain } = require("./lib/utils");
const crypto = require("crypto");

module.exports = {
  startPublishAssets,
};

async function startPublishAssets() {
  const assetsByName = new Map();
  /** @type {Octokit} */
  let octokit;
  let owner;
  let repo;
  let release;

  let initializePromise = null;
  let NODE_PRE_GYP_GITHUB_TOKEN;

  function getGithubKey() {
    if (NODE_PRE_GYP_GITHUB_TOKEN) {
      return NODE_PRE_GYP_GITHUB_TOKEN;
    }

    NODE_PRE_GYP_GITHUB_TOKEN = process.env.NODE_PRE_GYP_GITHUB_TOKEN;
    if (!NODE_PRE_GYP_GITHUB_TOKEN) {
      try {
        NODE_PRE_GYP_GITHUB_TOKEN = fs
          .readFileSync(path.resolve(__dirname, "../.github-key"), "utf8")
          .replace("\n", "")
          .replace("\r", "")
          .trim();
      } catch (e) {}
    }

    if (!NODE_PRE_GYP_GITHUB_TOKEN) {
      throw new Error("NODE_PRE_GYP_GITHUB_TOKEN environment variable not set and .github-key file not found");
    }

    return NODE_PRE_GYP_GITHUB_TOKEN;
  }

  function checkGithubKey() {
    getGithubKey();
  }

  async function initialize() {
    const branchName = execSync("git rev-parse --abbrev-ref HEAD")
      .toString()
      .replace(/[\n\r]/g, "")
      .trim();

    console.log(colors.cyanBright(`branchName: ${branchName}`));

    if (!branchName.includes("deploy")) {
      throw new Error('Can deploy only from "deploy" branches.');
    }

    if (
      branchName.includes("alpha") &&
      (!packageJson.version.includes("alpha") || packageJson.version.includes("beta"))
    ) {
      throw new Error("This is not an alpha or beta version, aborting.");
    }

    const ownerRepo = packageJson.repository.url.match(/https?:\/\/([^/]+)\/(.*)(?=\.git)/i);
    const host = `api.${ownerRepo[1]}`;
    [owner, repo] = ownerRepo[2].split("/");

    octokit = new Octokit({
      baseUrl: `https://${host}`,
      auth: getGithubKey(),
      headers: { "user-agent": packageJson.name },
    });

    const { data: existingReleases } = await octokit.rest.repos.listReleases({ owner, repo });

    release = existingReleases.find((element) => element.tag_name === packageJson.version);

    if (!release) {
      release = (
        await octokit.rest.repos.createRelease({
          host,
          owner,
          repo,
          tag_name: packageJson.version,
          target_commitish: branchName,
          name: `v${packageJson.version}`,
          body: `${packageJson.name} ${packageJson.version}`,
          draft: true,
          prerelease: /[a-zA-Z]/.test(packageJson.version),
        })
      ).data;
      console.log(colors.yellow(`Creating new release ${packageJson.version}`));
    } else {
      console.log(colors.yellow(`Using existing release ${packageJson.version}`));
    }

    if (!release.draft) {
      if (process.argv.includes("--overwrite")) {
        console.log();
        console.warn(colors.yellowBright(`⚠️ WARNING: Overwriting release ${packageJson.version}`));
        console.log();
      } else {
        throw new Error(
          `Release ${packageJson.version} was already published, aborting. Run with the argument "--overwrite" to overwrite existing assets.`,
        );
      }
    }

    console.log();
    console.warn(
      colors.yellowBright(
        `⚠️ WARNING: Release ${packageJson.version} is a draft release, YOU MUST MANUALLY PUBLISH THIS DRAFT IN GITHUB.`,
      ),
    );
    console.log();

    for (const asset of (release && release.assets) || []) {
      assetsByName.set(asset.name, asset);
    }
  }

  const upload = async (filePath = path.resolve(__dirname, "../build/stage")) => {
    if (!initializePromise) {
      initializePromise = initialize();
    }
    await initializePromise;

    filePath = path.resolve(filePath);

    const stat = await fs.promises.lstat(filePath);
    if (stat.isDirectory()) {
      const files = await fs.promises.readdir(filePath);
      await Promise.all(files.map((file) => upload(path.resolve(filePath, file))));
      return;
    }

    const name = path.basename(filePath);

    const data = await fs.promises.readFile(filePath);
    const uploadFileHash = crypto.createHash("sha256").update(data).digest("hex");

    console.log(colors.cyanBright(`* file ${name} hash: ${uploadFileHash}`));

    const foundAsset = assetsByName.get(name);
    if (foundAsset) {
      const result = await octokit.request({
        method: "GET",
        url: foundAsset.url,
        headers: {
          accept: "application/octet-stream",
        },
      });

      const downloadedFileHash = crypto.createHash("sha256").update(Buffer.from(result.data)).digest("hex");

      if (downloadedFileHash === uploadFileHash) {
        console.log(colors.yellow(`Skipping upload of ${name} because it already exists and is the same`));
        return;
      }

      console.log(colors.yellow(`Deleting asset ${foundAsset.name} id:${foundAsset.id} to replace it`));
      await octokit.rest.repos.deleteReleaseAsset({
        owner,
        repo,
        asset_id: foundAsset.id,
      });
    }

    console.log(colors.yellow(`Uploading asset ${name} ${stat.size} bytes`));
    await octokit.rest.repos.uploadReleaseAsset({
      owner,
      repo,
      release_id: release.id,
      tag_name: release.tag_name,
      name,
      data,
    });
  };

  return {
    checkGithubKey,
    upload,
  };
}

if (require.main === module) {
  runMain(async () => {
    const upload = await startPublishAssets();
    await upload();
  }, "node-pre-gyp-publish");
}
