import RoaringBitmap32 from "../../RoaringBitmap32";
import { expect, use as chaiUse } from "chai";
import path from "path";
import fs from "fs";

const tmpDir = path.resolve(__dirname, "..", "..", ".tmp", "tests");

chaiUse(require("chai-as-promised"));

describe("RoaringBitmap32 file serialization", () => {
  before(() => {
    if (!fs.existsSync(tmpDir)) {
      fs.mkdirSync(tmpDir, { recursive: true });
    }
  });

  it("serialize and deserialize in various formats", async () => {
    for (const format of ["portable", "croaring", "unsafe_frozen_croaring"] as const) {
      const tmpFilePath = path.resolve(tmpDir, `test-ϴϮ-${format}.bin`);
      const data = [1, 2, 3, 100, 0xfffff, 0xffffffff];
      await new RoaringBitmap32(data).serializeFileAsync(tmpFilePath, format);
      expect((await RoaringBitmap32.deserializeFileAsync(tmpFilePath, format)).toArray()).to.deep.equal(data);
    }
  });

  it("serializeFileAsync truncates file if it already exists", async () => {
    const tmpFilePath = path.resolve(tmpDir, `test-truncate.bin`);
    await fs.promises.writeFile(tmpFilePath, Buffer.alloc(10000));
    await new RoaringBitmap32([1, 2, 3]).serializeFileAsync(tmpFilePath, "portable");
    expect((await RoaringBitmap32.deserializeFileAsync(tmpFilePath, "portable")).toArray()).to.deep.equal([1, 2, 3]);
  });

  it("throws ENOENT if file does not exist", async () => {
    const tmpFilePath = path.resolve(tmpDir, `test-ENOENT.bin`);
    let error: any;
    try {
      await RoaringBitmap32.deserializeFileAsync(tmpFilePath, "portable");
    } catch (e) {
      error = e;
    }
    expect(error).to.be.an.instanceOf(Error);
    expect(error.message).to.match(/^ENOENT, No such file or directory/);
    expect(error.code).to.equal("ENOENT");
    expect(error.syscall).to.equal("open");
    expect(error.path).to.equal(tmpFilePath);
  });
});
