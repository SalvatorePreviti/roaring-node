import RoaringBitmap32 from "../../RoaringBitmap32";
import { expect, use as chaiUse } from "chai";
import path from "path";
import fs from "fs";

const tmpDir = path.resolve(__dirname, "..", "..", ".tmp", "tests");

chaiUse(require("chai-as-promised"));

describe.only("RoaringBitmap32 file serialization", () => {
  before(() => {
    if (!fs.existsSync(tmpDir)) {
      fs.mkdirSync(tmpDir, { recursive: true });
    }
  });

  it.skip("serialize and deserialize in various formats", async () => {
    for (const format of ["portable", "croaring", "unsafe_frozen_croaring"] as const) {
      const tmpFilePath = path.resolve(tmpDir, `test-ϴϮ-${format}.bin`);
      const data = [1, 2, 3, 100, 0xfffff, 0xffffffff];
      await new RoaringBitmap32(data).serializeFileAsync(tmpFilePath, format);
      expect((await RoaringBitmap32.deserializeFileAsync(tmpFilePath, format)).toArray()).to.deep.equal(data);
    }
  });
});
