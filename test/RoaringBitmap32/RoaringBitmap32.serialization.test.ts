import RoaringBitmap32 from "../../RoaringBitmap32";
import { expect } from "chai";

describe("RoaringBitmap32 serialization", () => {
  const data = [1, 2, 3, 4, 5, 6, 100, 101, 105, 109, 0x7fffffff, 0xfffffffe, 0xffffffff];

  describe("getSerializationSizeInBytes", () => {
    it("throws if the argument is not a valid format", () => {
      const bitmap = new RoaringBitmap32(data);
      const expectedError = "RoaringBitmap32::getSerializationSizeInBytes format argument was invalid";
      expect(() => bitmap.getSerializationSizeInBytes(undefined as any)).to.throw(expectedError);
      expect(() => bitmap.getSerializationSizeInBytes(null as any)).to.throw(expectedError);
      expect(() => bitmap.getSerializationSizeInBytes("foo" as any)).to.throw(expectedError);
      expect(() => bitmap.getSerializationSizeInBytes(0 as any)).to.throw(expectedError);
      expect(() => bitmap.getSerializationSizeInBytes(1 as any)).to.throw(expectedError);
    });

    it("returns standard value for empty bitmap (non portable)", () => {
      const bitmap = new RoaringBitmap32();
      expect(bitmap.getSerializationSizeInBytes(false)).eq(5);
      expect(bitmap.getSerializationSizeInBytes("croaring")).eq(5);
    });

    it("returns standard value for empty bitmap (portable)", () => {
      const bitmap = new RoaringBitmap32();
      expect(bitmap.getSerializationSizeInBytes(true)).eq(8);
      expect(bitmap.getSerializationSizeInBytes("portable")).eq(8);
    });

    it("returns a value for frozen croaring", () => {
      expect(new RoaringBitmap32([1, 2, 3]).getSerializationSizeInBytes("unsafe_frozen_croaring")).gt(0);
    });

    it("returns the correct amount of bytes (non portable)", () => {
      const bitmap = new RoaringBitmap32(data);
      expect(bitmap.getSerializationSizeInBytes(false)).eq(bitmap.serialize(false).byteLength);
      expect(bitmap.getSerializationSizeInBytes("croaring")).eq(bitmap.serialize("croaring").byteLength);
      bitmap.runOptimize();
      bitmap.shrinkToFit();
      expect(bitmap.getSerializationSizeInBytes(false)).eq(bitmap.serialize(false).byteLength);
      expect(bitmap.getSerializationSizeInBytes("croaring")).eq(bitmap.serialize("croaring").byteLength);
    });

    it("returns the correct amount of bytes (portable)", () => {
      const bitmap = new RoaringBitmap32(data);
      expect(bitmap.getSerializationSizeInBytes(true)).eq(bitmap.serialize(true).byteLength);
      bitmap.runOptimize();
      bitmap.shrinkToFit();
      expect(bitmap.getSerializationSizeInBytes(true)).eq(bitmap.serialize(true).byteLength);
    });
  });

  describe("serialize", () => {
    it("returns a Buffer", () => {
      expect(new RoaringBitmap32().serialize(false)).to.be.instanceOf(Buffer);
    });

    it("returns standard value for empty bitmap (non portable)", () => {
      const bitmap = new RoaringBitmap32();
      expect(Array.from(bitmap.serialize(false))).deep.equal([1, 0, 0, 0, 0]);
    });

    it("returns standard value for empty bitmap (portable)", () => {
      expect(Array.from(new RoaringBitmap32().serialize(true))).deep.equal([58, 48, 0, 0, 0, 0, 0, 0]);
    });

    it("can serialize to a SharedArrayBuffer", () => {
      const bitmap = new RoaringBitmap32(data);
      const output = new SharedArrayBuffer(bitmap.getSerializationSizeInBytes(true));
      const buffer = bitmap.serialize("portable", output);
      expect(buffer.buffer).to.eq(output);
      expect(Array.from(new Uint8Array(buffer))).deep.equal(Array.from(bitmap.serialize(true)));
    });
  });

  describe("deserialize", () => {
    it("deserializes zero length buffer (non portable)", () => {
      const bitmap = new RoaringBitmap32([1, 2, 3]);
      bitmap.deserialize(Buffer.from([]), false);
      expect(bitmap.size).eq(0);
      expect(bitmap.isEmpty).eq(true);
      bitmap.deserialize(Buffer.from([]), "croaring");
      expect(bitmap.size).eq(0);
      expect(bitmap.isEmpty).eq(true);
    });

    it("deserializes empty bitmap (non portable)", () => {
      const bitmap = new RoaringBitmap32([1, 2, 3]);
      bitmap.deserialize(Buffer.from([1, 0, 0, 0, 0]), false);
      expect(bitmap.size).eq(0);
      expect(bitmap.isEmpty).eq(true);
      bitmap.deserialize(Buffer.from([1, 0, 0, 0, 0]), "croaring");
      expect(bitmap.size).eq(0);
      expect(bitmap.isEmpty).eq(true);
    });

    it("deserializes empty bitmap (non portable)", () => {
      const bitmap = new RoaringBitmap32([1, 2, 3]);
      bitmap.deserialize(Buffer.from([1, 0, 0, 0, 0]), false);
      expect(bitmap.size).eq(0);
      expect(bitmap.isEmpty).eq(true);
    });

    it("deserializes empty bitmap (portable)", () => {
      const bitmap = new RoaringBitmap32([1, 2]);
      bitmap.deserialize(Buffer.from([58, 48, 0, 0, 0, 0, 0, 0]), true);
      expect(bitmap.size).eq(0);
      expect(bitmap.isEmpty).eq(true);
      bitmap.deserialize(Buffer.from([58, 48, 0, 0, 0, 0, 0, 0]), "portable");
      expect(bitmap.size).eq(0);
      expect(bitmap.isEmpty).eq(true);
    });

    it("deserializes zero length buffer (portable)", () => {
      const bitmap = new RoaringBitmap32([1, 2]);
      bitmap.deserialize(Buffer.from([]), true);
      expect(bitmap.size).eq(0);
      expect(bitmap.isEmpty).eq(true);
      bitmap.deserialize(Buffer.from([]), "portable");
      expect(bitmap.size).eq(0);
      expect(bitmap.isEmpty).eq(true);
    });

    it("is able to deserialize test data", () => {
      const testDataSerialized = require("./data/serialized.json");

      let total = 0;
      for (const s of testDataSerialized) {
        const bitmap = RoaringBitmap32.deserialize(Buffer.from(s, "base64"), false);
        const size = bitmap.size;
        if (size !== 0) {
          expect(bitmap.has(bitmap.minimum())).eq(true);
          expect(bitmap.has(bitmap.maximum())).eq(true);
        }
        total += size;
      }
      expect(total).eq(68031);
    });
  });

  describe("deserialize static", () => {
    it("deserializes zero length buffer (non portable)", () => {
      let bitmap = RoaringBitmap32.deserialize(Buffer.from([]), false);
      expect(bitmap.size).eq(0);
      expect(bitmap.isEmpty).eq(true);
      bitmap = RoaringBitmap32.deserialize(Buffer.from([]), "portable");
      expect(bitmap.size).eq(0);
      expect(bitmap.isEmpty).eq(true);
    });

    it("deserializes zero length buffer (portable)", () => {
      let bitmap = RoaringBitmap32.deserialize(Buffer.from([]), true);
      expect(bitmap.size).eq(0);
      expect(bitmap.isEmpty).eq(true);
      bitmap = RoaringBitmap32.deserialize(Buffer.from([]), "portable");
      expect(bitmap.size).eq(0);
      expect(bitmap.isEmpty).eq(true);
    });

    it("deserializes empty bitmap (non portable)", () => {
      const bitmap = RoaringBitmap32.deserialize(Buffer.from([1, 0, 0, 0, 0]), false);
      expect(bitmap.size).eq(0);
      expect(bitmap.isEmpty).eq(true);
    });

    it("deserializes empty bitmap (non portable)", () => {
      const bitmap = RoaringBitmap32.deserialize(Buffer.from([1, 0, 0, 0, 0]), false);
      expect(bitmap.size).eq(0);
      expect(bitmap.isEmpty).eq(true);
    });

    it("deserializes empty bitmap (non portable)", () => {
      let bitmap = RoaringBitmap32.deserialize(Buffer.from([1, 0, 0, 0, 0]), false);
      expect(bitmap.size).eq(0);
      expect(bitmap.isEmpty).eq(true);
      bitmap = RoaringBitmap32.deserialize(Buffer.from([1, 0, 0, 0, 0]), "croaring");
      expect(bitmap.size).eq(0);
      expect(bitmap.isEmpty).eq(true);
    });

    it("deserializes empty bitmap (portable)", () => {
      let bitmap = RoaringBitmap32.deserialize(Buffer.from([58, 48, 0, 0, 0, 0, 0, 0]), true);
      expect(bitmap.size).eq(0);
      expect(bitmap.isEmpty).eq(true);
      bitmap = RoaringBitmap32.deserialize(Buffer.from([58, 48, 0, 0, 0, 0, 0, 0]), "portable");
      expect(bitmap.size).eq(0);
      expect(bitmap.isEmpty).eq(true);
    });
  });

  describe("serialize, deserialize", () => {
    it("is able to serialize and deserialize data (non portable)", () => {
      const a = new RoaringBitmap32(data);
      const b = RoaringBitmap32.deserialize(a.serialize(false), false);
      expect(b.toArray()).deep.equal(data);
    });

    it("is able to serialize and deserialize data (portable)", () => {
      const a = new RoaringBitmap32(data);
      const b = RoaringBitmap32.deserialize(a.serialize(true), true);
      expect(b.toArray()).deep.equal(data);
    });
  });

  describe("serialize to buffer", () => {
    it("throws if buffer is not a valid buffer", () => {
      const bitmap = new RoaringBitmap32(data);
      expect(() => bitmap.serialize(false, {} as any)).to.throw();
      expect(() => bitmap.serialize(false, 1 as any)).to.throw();
      expect(() => bitmap.serialize(false, "test" as any)).to.throw();
      expect(() => bitmap.serialize(false, [1, 2, 3] as any)).to.throw();
    });

    it("throws if buffer is too small", () => {
      const bitmap = new RoaringBitmap32(data);
      const buffer = Buffer.alloc(bitmap.getSerializationSizeInBytes(false) - 1);
      expect(() => bitmap.serialize(false, buffer)).to.throw();
    });

    it("throws if buffer is too small (portable)", () => {
      const bitmap = new RoaringBitmap32(data);
      const buffer = Buffer.alloc(bitmap.getSerializationSizeInBytes(true) - 1);
      expect(() => bitmap.serialize(true, buffer)).to.throw();
    });

    it("serializes to buffer (non portable)", () => {
      const bitmap = new RoaringBitmap32(data);
      const buffer = Buffer.alloc(bitmap.getSerializationSizeInBytes(false));
      expect(bitmap.serialize(false, buffer)).to.eq(buffer);
      expect(RoaringBitmap32.deserialize(buffer, false).toArray()).to.deep.eq(data);
    });

    it("serializes to buffer (portable)", () => {
      const bitmap = new RoaringBitmap32(data);
      const buffer = Buffer.alloc(bitmap.getSerializationSizeInBytes(true));
      expect(bitmap.serialize(true, buffer)).to.eq(buffer);
      expect(RoaringBitmap32.deserialize(buffer, true).toArray()).to.deep.eq(data);
    });

    it("serializes to buffer (portable), inverted arguments", () => {
      const bitmap = new RoaringBitmap32(data);
      const buffer = Buffer.alloc(bitmap.getSerializationSizeInBytes(true));
      expect(bitmap.serialize(buffer, true)).to.eq(buffer);
      expect(RoaringBitmap32.deserialize(buffer, true).toArray()).to.deep.eq(data);
    });

    it("handles offset correctly and deserialize correctly", () => {
      const bitmap = new RoaringBitmap32(data);
      const serlength = bitmap.getSerializationSizeInBytes(true);
      const buffer = Buffer.alloc(serlength + 30);
      const serialized = bitmap.serialize("portable", Buffer.from(buffer.buffer, 10));
      expect(Array.from(serialized)).to.deep.eq(Array.from(Buffer.from(buffer.buffer, 10, serlength)));
      expect(RoaringBitmap32.deserialize(serialized, true).toArray()).to.deep.eq(data);
    });
  });

  describe("serializeAsync", () => {
    it("serializes to buffer (non portable)", async () => {
      const bitmap = new RoaringBitmap32(data);
      const buffer = Buffer.alloc(bitmap.getSerializationSizeInBytes(false));
      expect(await bitmap.serializeAsync(false, buffer)).to.eq(buffer);
      expect(RoaringBitmap32.deserialize(buffer, false).toArray()).to.deep.eq(data);
    });

    it("serializes to buffer (portable)", async () => {
      const bitmap = new RoaringBitmap32(data);
      const buffer = Buffer.alloc(bitmap.getSerializationSizeInBytes(true));
      expect(await bitmap.serializeAsync(true, buffer)).to.eq(buffer);
      expect(RoaringBitmap32.deserialize(buffer, true).toArray()).to.deep.eq(data);
    });

    it("handles offset correctly and deserialize correctly", async () => {
      const bitmap = new RoaringBitmap32(data);
      const serlength = bitmap.getSerializationSizeInBytes(true);
      const buffer = Buffer.alloc(serlength + 30);
      const offsetted = await bitmap.serializeAsync(true, Buffer.from(buffer.buffer, 10));
      expect(Array.from(offsetted)).to.deep.eq(Array.from(Buffer.from(buffer.buffer, 10, serlength)));
      expect(offsetted.buffer).to.eq(buffer.buffer);
      expect(RoaringBitmap32.deserialize(Buffer.from(buffer.buffer, 10), true).toArray()).to.deep.eq(data);
    });

    it("throws if buffer is not a valid buffer", async () => {
      const bitmap = new RoaringBitmap32(data);
      await expect(bitmap.serializeAsync(false, {} as any)).to.be.rejected;
      await expect(bitmap.serializeAsync(false, 1 as any)).to.be.rejected;
      await expect(bitmap.serializeAsync(false, "test" as any)).to.be.rejected;
      await expect(bitmap.serializeAsync(false, [1, 2, 3] as any)).to.be.rejected;
    });

    it("throws if buffer is too small", async () => {
      const bitmap = new RoaringBitmap32(data);
      const buffer = Buffer.alloc(bitmap.getSerializationSizeInBytes(false) - 1);
      await expect(bitmap.serializeAsync(false, buffer)).to.be.rejected;
    });

    it("throws if buffer is too small (portable)", async () => {
      const bitmap = new RoaringBitmap32(data);
      const buffer = Buffer.alloc(bitmap.getSerializationSizeInBytes(true) - 1);
      await expect(bitmap.serializeAsync(true, buffer)).to.be.rejected;
    });

    it("serializes to buffer (non portable)", async () => {
      const bitmap = new RoaringBitmap32(data);
      const buffer = Buffer.alloc(bitmap.getSerializationSizeInBytes(false));
      expect(await bitmap.serializeAsync(false, buffer)).to.eq(buffer);
      expect(RoaringBitmap32.deserialize(buffer, false).toArray()).to.deep.eq(data);
    });

    it("serializes to buffer (portable)", async () => {
      const bitmap = new RoaringBitmap32(data);
      const buffer = Buffer.alloc(bitmap.getSerializationSizeInBytes(true));
      expect(await bitmap.serializeAsync(true, buffer)).to.eq(buffer);
      expect(RoaringBitmap32.deserialize(buffer, true).toArray()).to.deep.eq(data);
    });

    it("handles offset correctly and deserialize correctly", async () => {
      const bitmap = new RoaringBitmap32(data);
      const serlength = bitmap.getSerializationSizeInBytes(true);
      const buffer = Buffer.alloc(serlength + 20);
      const offsetted = await bitmap.serializeAsync(true, Buffer.from(buffer.buffer, 10));
      expect(Array.from(offsetted)).to.deep.eq(Array.from(Buffer.from(buffer.buffer, 10, serlength)));
      expect(offsetted.buffer).to.eq(buffer.buffer);
      expect(RoaringBitmap32.deserialize(Buffer.from(buffer.buffer, 10), true).toArray()).to.deep.eq(data);
    });
  });

  it("serialize and deserialize in various formats", async () => {
    for (const format of ["portable", "croaring", "unsafe_frozen_croaring"] as const) {
      const smallArray = [1, 2, 3, 100, 0xfffff, 0xffffffff];
      const serialized = await new RoaringBitmap32(smallArray).serializeAsync(format);
      expect((await RoaringBitmap32.deserializeAsync(serialized, format)).toArray()).to.deep.equal(smallArray);
    }
  });
});
