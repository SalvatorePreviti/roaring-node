// lint-staged.config.js

const quoteFiles = (files) => files.map((file) => JSON.stringify(file));

module.exports = {
  "*": (files) => {
    if (!files.length) {
      return [];
    }
    const args = quoteFiles(files).join(" ");
    return `biome check --write --files-ignore-unknown=true ${args}`;
  },
};
