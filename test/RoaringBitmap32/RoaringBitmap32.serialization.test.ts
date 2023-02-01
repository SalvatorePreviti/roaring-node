import RoaringBitmap32 from "../../RoaringBitmap32";
import { SerializationFormat } from "../../";
import { expect } from "chai";

describe("RoaringBitmap32 serialization", () => {
  const data = [1, 2, 3, 4, 5, 6, 100, 101, 105, 109, 0x7fffffff, 0xfffffffe, 0xffffffff];

  describe("RoaringBitmap32SerializationFormat", () => {
    it("should have the right values", () => {
      expect(SerializationFormat.croaring).eq("croaring");
      expect(SerializationFormat.portable).eq("portable");
      expect(SerializationFormat.frozen_croaring).eq("frozen_croaring");

      expect(Object.values(SerializationFormat)).to.deep.eq(["croaring", "portable", "frozen_croaring"]);

      expect(RoaringBitmap32.SerializationFormat).to.eq(SerializationFormat);

      expect(new RoaringBitmap32().SerializationFormat).to.eq(SerializationFormat);
    });
  });

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
      expect(new RoaringBitmap32([1, 2, 3]).getSerializationSizeInBytes("frozen_croaring")).gt(0);
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
  });

  describe("deserialize", () => {
    it("deserializes zero length buffer (non portable)", () => {
      const bitmap = new RoaringBitmap32([1, 2, 3]);
      bitmap.deserialize(Buffer.from([]), false);
      expect(bitmap.size).eq(0);
      expect(bitmap.isEmpty).eq(true);
    });

    it("deserializes empty bitmap (non portable)", () => {
      const bitmap = new RoaringBitmap32([1, 2, 3]);
      bitmap.deserialize(Buffer.from([1, 0, 0, 0, 0]), false);
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
    });

    it("deserializes zero length buffer (portable)", () => {
      const bitmap = new RoaringBitmap32([1, 2]);
      bitmap.deserialize(Buffer.from([]), true);
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
      const bitmap = RoaringBitmap32.deserialize(Buffer.from([]), false);
      expect(bitmap.size).eq(0);
      expect(bitmap.isEmpty).eq(true);
    });

    it("deserializes zero length buffer (portable)", () => {
      const bitmap = RoaringBitmap32.deserialize(Buffer.from([]), true);
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
      const bitmap = RoaringBitmap32.deserialize(Buffer.from([1, 0, 0, 0, 0]), false);
      expect(bitmap.size).eq(0);
      expect(bitmap.isEmpty).eq(true);
    });

    it("deserializes empty bitmap (portable)", () => {
      const bitmap = RoaringBitmap32.deserialize(Buffer.from([58, 48, 0, 0, 0, 0, 0, 0]), true);
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

    it("throws if offset is not a number, infinity, NaN or negative or too big", () => {
      const bitmap = new RoaringBitmap32(data);
      const buffer = Buffer.alloc(bitmap.getSerializationSizeInBytes(true));
      expect(() => bitmap.serialize(true, buffer, 10.1)).to.throw();
      expect(() => bitmap.serialize(true, buffer, Infinity)).to.throw();
      expect(() => bitmap.serialize(true, buffer, NaN)).to.throw();
      expect(() => bitmap.serialize(true, buffer, -1)).to.throw();
      expect(() => bitmap.serialize(true, buffer, Number.MAX_SAFE_INTEGER)).to.throw();
      expect(() => bitmap.serialize(true, buffer, buffer.length - 1)).to.throw();
    });

    it("serializes to buffer (non portable)", () => {
      const bitmap = new RoaringBitmap32(data);
      const buffer = Buffer.alloc(bitmap.getSerializationSizeInBytes(false));
      bitmap.serialize(false, buffer);
      expect(RoaringBitmap32.deserialize(buffer, false).toArray()).to.deep.eq(data);
    });

    it("serializes to buffer (portable)", () => {
      const bitmap = new RoaringBitmap32(data);
      const buffer = Buffer.alloc(bitmap.getSerializationSizeInBytes(true));
      bitmap.serialize(true, buffer);
      expect(RoaringBitmap32.deserialize(buffer, true).toArray()).to.deep.eq(data);
    });

    it("handles offset correctly and deserialize correctly", () => {
      const bitmap = new RoaringBitmap32(data);
      const buffer = Buffer.alloc(bitmap.getSerializationSizeInBytes(true) + 10);
      bitmap.serialize(true, buffer, 10);
      expect(RoaringBitmap32.deserialize(Buffer.from(buffer, 10), true).toArray()).to.deep.eq(data);
    });
  });
});
