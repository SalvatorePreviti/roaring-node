import fs from "node:fs";
import path from "node:path";
import { beforeAll, describe, expect, it } from "vitest";
import RoaringBitmap32 from "../../RoaringBitmap32";
import type { FileSerializationDeserializationFormatType } from "../helpers/roaring";
import { FileDeserializationFormat, FileSerializationFormat } from "../helpers/roaring";

const tmpDir = path.resolve(__dirname, "..", "..", ".tmp", "tests");

describe("RoaringBitmap32 file serialization", () => {
  beforeAll(() => {
    if (!fs.existsSync(tmpDir)) {
      fs.mkdirSync(tmpDir, { recursive: true });
    }
  });

  describe("FileSerializationFormat", () => {
    it("should have the right values", () => {
      expect(FileSerializationFormat.croaring).eq("croaring");
      expect(FileSerializationFormat.portable).eq("portable");
      expect(FileSerializationFormat.uint32_array).eq("uint32_array");
      expect(FileSerializationFormat.unsafe_frozen_croaring).eq("unsafe_frozen_croaring");
      expect(FileSerializationFormat.comma_separated_values).eq("comma_separated_values");
      expect(FileSerializationFormat.tab_separated_values).eq("tab_separated_values");
      expect(FileSerializationFormat.newline_separated_values).eq("newline_separated_values");
      expect(FileSerializationFormat.json_array).eq("json_array");

      expect(Object.values(FileSerializationFormat)).to.deep.eq([
        "croaring",
        "portable",
        "unsafe_frozen_croaring",
        "uint32_array",
        "comma_separated_values",
        "tab_separated_values",
        "newline_separated_values",
        "json_array",
      ]);

      expect(RoaringBitmap32.FileSerializationFormat).to.eq(FileSerializationFormat);

      expect(new RoaringBitmap32().FileSerializationFormat).to.eq(FileSerializationFormat);
    });
  });

  describe("FileDeserializationFormat", () => {
    it("should have the right values", () => {
      expect(FileDeserializationFormat.croaring).eq("croaring");
      expect(FileDeserializationFormat.portable).eq("portable");
      expect(FileDeserializationFormat.unsafe_frozen_croaring).eq("unsafe_frozen_croaring");
      expect(FileDeserializationFormat.unsafe_frozen_portable).eq("unsafe_frozen_portable");
      expect(FileDeserializationFormat.comma_separated_values).eq("comma_separated_values");
      expect(FileDeserializationFormat.tab_separated_values).eq("tab_separated_values");
      expect(FileDeserializationFormat.newline_separated_values).eq("newline_separated_values");
      expect(FileDeserializationFormat.json_array).eq("json_array");

      expect(Object.values(FileDeserializationFormat)).to.deep.eq([
        "croaring",
        "portable",
        "unsafe_frozen_croaring",
        "unsafe_frozen_portable",
        "uint32_array",
        "comma_separated_values",
        "tab_separated_values",
        "newline_separated_values",
        "json_array",
      ]);

      expect(RoaringBitmap32.FileDeserializationFormat).to.eq(FileDeserializationFormat);

      expect(new RoaringBitmap32().FileDeserializationFormat).to.eq(FileDeserializationFormat);
    });
  });

  it("serialize and deserialize empty bitmaps in various formats", async () => {
    const formats: FileSerializationDeserializationFormatType[] = [
      "portable",
      "croaring",
      "unsafe_frozen_croaring",
      "uint32_array",
      "comma_separated_values",
      "tab_separated_values",
      "newline_separated_values",
      "json_array",
    ];
    for (const format of formats) {
      const tmpFilePath = path.resolve(tmpDir, `test-ϴ-${format}.bin`);
      await new RoaringBitmap32().serializeFileAsync(tmpFilePath, format);
      expect((await RoaringBitmap32.deserializeFileAsync(tmpFilePath, format)).toArray()).to.deep.equal([]);
      expect(RoaringBitmap32.deserializeFile(tmpFilePath, format).toArray()).to.deep.equal([]);
    }
  });

  it("serialize and deserialize in various formats", async () => {
    for (const format of ["portable", "croaring", "unsafe_frozen_croaring", "uint32_array"] as const) {
      const tmpFilePath = path.resolve(tmpDir, `test-1-${format}.bin`);
      const data = [1, 2, 3, 100, 0xfffff, 0xffffffff];
      await new RoaringBitmap32(data).serializeFileAsync(tmpFilePath, format);
      expect((await RoaringBitmap32.deserializeFileAsync(tmpFilePath, format)).toArray()).to.deep.equal(data);
      expect(RoaringBitmap32.deserializeFile(tmpFilePath, format).toArray()).to.deep.equal(data);
    }
  });

  it("serializeFileAsync truncates file if it already exists", async () => {
    const tmpFilePath = path.resolve(tmpDir, `test-truncate.bin`);
    await fs.promises.writeFile(tmpFilePath, Buffer.alloc(10000));
    await new RoaringBitmap32([1, 2, 3]).serializeFileAsync(tmpFilePath, "portable");
    expect((await RoaringBitmap32.deserializeFileAsync(tmpFilePath, "portable")).toArray()).to.deep.equal([1, 2, 3]);
    expect(RoaringBitmap32.deserializeFile(tmpFilePath, "portable").toArray()).to.deep.equal([1, 2, 3]);
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

    let syncError: any;
    try {
      RoaringBitmap32.deserializeFile(tmpFilePath, "portable");
    } catch (e) {
      syncError = e;
    }
    expect(syncError).to.be.an.instanceOf(Error);
    expect(syncError.message).to.match(/^ENOENT, No such file or directory/);
    expect(syncError.code).to.equal("ENOENT");
    expect(syncError.syscall).to.equal("open");
    expect(syncError.path).to.equal(tmpFilePath);
  });

  it("serializes to comma_separated_values", async () => {
    const tmpFilePath = path.resolve(tmpDir, `test-csv.csv`);
    const bmp = new RoaringBitmap32([1, 2, 3, 100, 14120, 3481983]);
    bmp.addRange(0x100, 0x120);
    await bmp.serializeFileAsync(tmpFilePath, "comma_separated_values");
    const text = await fs.promises.readFile(tmpFilePath, "utf8");
    expect(text).to.equal(
      "1,2,3,100,256,257,258,259,260,261,262,263,264,265,266,267,268,269,270,271,272,273,274,275,276,277,278,279,280,281,282,283,284,285,286,287,14120,3481983",
    );
  });

  it("serializes to newline_separated_values", async () => {
    const tmpFilePath = path.resolve(tmpDir, `test-nlf.csv`);
    const bmp = new RoaringBitmap32([1, 2, 3, 100, 14120, 3481983]);
    bmp.addRange(0x100, 0x120);
    await bmp.serializeFileAsync(tmpFilePath, "newline_separated_values");
    const text = await fs.promises.readFile(tmpFilePath, "utf8");
    expect(text).to.equal(
      "1,2,3,100,256,257,258,259,260,261,262,263,264,265,266,267,268,269,270,271,272,273,274,275,276,277,278,279,280,281,282,283,284,285,286,287,14120,3481983,".replace(
        /,/g,
        "\n",
      ),
    );
  });

  it("serializes to tab_separated_values", async () => {
    const tmpFilePath = path.resolve(tmpDir, `test-ntab.csv`);
    const bmp = new RoaringBitmap32([1, 2, 3, 100, 14120, 3481983]);
    bmp.addRange(0x100, 0x120);
    await bmp.serializeFileAsync(tmpFilePath, "tab_separated_values");
    const text = await fs.promises.readFile(tmpFilePath, "utf8");
    expect(text).to.equal(
      "1,2,3,100,256,257,258,259,260,261,262,263,264,265,266,267,268,269,270,271,272,273,274,275,276,277,278,279,280,281,282,283,284,285,286,287,14120,3481983".replace(
        /,/g,
        "\t",
      ),
    );
  });

  it("serializes to an empty json array", async () => {
    const tmpFilePath = path.resolve(tmpDir, `test-json-array-empty.csv`);
    await new RoaringBitmap32().serializeFileAsync(tmpFilePath, "json_array");
    expect(await fs.promises.readFile(tmpFilePath, "utf8")).to.equal("[]");
  });

  it("serializes to a json array", async () => {
    const tmpFilePath = path.resolve(tmpDir, `test-json-array.csv`);
    const bmp = new RoaringBitmap32([1, 2, 3, 100, 14120, 3481983]);
    bmp.addRange(0x100, 0x120);
    await bmp.serializeFileAsync(tmpFilePath, "json_array");
    const text = await fs.promises.readFile(tmpFilePath, "utf8");
    expect(text).to.equal(
      "[1,2,3,100,256,257,258,259,260,261,262,263,264,265,266,267,268,269,270,271,272,273,274,275,276,277,278,279,280,281,282,283,284,285,286,287,14120,3481983]",
    );
  });
});
