import RoaringBitmap32 from "../../RoaringBitmap32";
import { DeserializationFormat, SerializationFormat } from "../..";
import { expect, describe, it } from "vitest";

describe("RoaringBitmap32 serialization", () => {
  const data = [1, 2, 3, 4, 5, 6, 100, 101, 105, 109, 0x7fffffff, 0xfffffffe, 0xffffffff];

  describe("SerializationFormat", () => {
    it("should have the right values", () => {
      expect(SerializationFormat.croaring).eq("croaring");
      expect(SerializationFormat.portable).eq("portable");
      expect(SerializationFormat.unsafe_frozen_croaring).eq("unsafe_frozen_croaring");
      expect(SerializationFormat.uint32_array).eq("uint32_array");

      expect(Object.values(SerializationFormat)).toEqual([
        "croaring",
        "portable",
        "unsafe_frozen_croaring",
        "uint32_array",
      ]);

      expect(RoaringBitmap32.SerializationFormat).eq(SerializationFormat);

      expect(new RoaringBitmap32().SerializationFormat).eq(SerializationFormat);
    });
  });

  describe("DeserializationFormat", () => {
    it("should have the right values", () => {
      expect(DeserializationFormat.croaring).eq("croaring");
      expect(DeserializationFormat.portable).eq("portable");
      expect(DeserializationFormat.unsafe_frozen_croaring).eq("unsafe_frozen_croaring");
      expect(DeserializationFormat.unsafe_frozen_portable).eq("unsafe_frozen_portable");
      expect(DeserializationFormat.comma_separated_values).eq("comma_separated_values");
      expect(DeserializationFormat.tab_separated_values).eq("tab_separated_values");
      expect(DeserializationFormat.newline_separated_values).eq("newline_separated_values");
      expect(DeserializationFormat.json_array).eq("json_array");

      expect(Object.values(DeserializationFormat)).toEqual([
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

      expect(RoaringBitmap32.DeserializationFormat).eq(DeserializationFormat);

      expect(new RoaringBitmap32().DeserializationFormat).eq(DeserializationFormat);
    });
  });

  describe("getSerializationSizeInBytes", () => {
    it("throws if the argument is not a valid format", () => {
      const bitmap = new RoaringBitmap32(data);
      const expectedError = "RoaringBitmap32::getSerializationSizeInBytes format argument was invalid";
      expect(() => bitmap.getSerializationSizeInBytes(undefined as any)).toThrow(expectedError);
      expect(() => bitmap.getSerializationSizeInBytes(null as any)).toThrow(expectedError);
      expect(() => bitmap.getSerializationSizeInBytes("foo" as any)).toThrow(expectedError);
      expect(() => bitmap.getSerializationSizeInBytes(0 as any)).toThrow(expectedError);
      expect(() => bitmap.getSerializationSizeInBytes(1 as any)).toThrow(expectedError);
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
      expect(new RoaringBitmap32().serialize(false)).toBeInstanceOf(Buffer);
    });

    it("returns standard value for empty bitmap (non portable)", () => {
      const bitmap = new RoaringBitmap32();
      expect(Array.from(bitmap.serialize(false))).toEqual([1, 0, 0, 0, 0]);
    });

    it("returns standard value for empty bitmap (portable)", () => {
      expect(Array.from(new RoaringBitmap32().serialize(true))).toEqual([58, 48, 0, 0, 0, 0, 0, 0]);
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

    it("deserializes empty bitmap (non portable) simple", () => {
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

    it("deserializes empty bitmap (non portable) simple", () => {
      const bitmap = RoaringBitmap32.deserialize(Buffer.from([1, 0, 0, 0, 0]), false);
      expect(bitmap.size).eq(0);
      expect(bitmap.isEmpty).eq(true);
    });

    it("deserializes empty bitmap (non portable) other", () => {
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
      expect(b.toArray()).toEqual(data);
    });

    it("is able to serialize and deserialize data (portable)", () => {
      const a = new RoaringBitmap32(data);
      const b = RoaringBitmap32.deserialize(a.serialize(true), true);
      expect(b.toArray()).toEqual(data);
    });
  });

  describe("serialize to buffer", () => {
    it("throws if buffer is not a valid buffer", () => {
      const bitmap = new RoaringBitmap32(data);
      expect(() => bitmap.serialize(false, {} as any)).toThrow();
      expect(() => bitmap.serialize(false, 1 as any)).toThrow();
      expect(() => bitmap.serialize(false, "test" as any)).toThrow();
      expect(() => bitmap.serialize(false, [1, 2, 3] as any)).toThrow();
    });

    it("throws if buffer is too small", () => {
      const bitmap = new RoaringBitmap32(data);
      const buffer = Buffer.alloc(bitmap.getSerializationSizeInBytes(false) - 1);
      expect(() => bitmap.serialize(false, buffer)).toThrow();
    });

    it("throws if buffer is too small (portable)", () => {
      const bitmap = new RoaringBitmap32(data);
      const buffer = Buffer.alloc(bitmap.getSerializationSizeInBytes(true) - 1);
      expect(() => bitmap.serialize(true, buffer)).toThrow();
    });

    it("serializes to buffer (non portable)", () => {
      const bitmap = new RoaringBitmap32(data);
      const buffer = Buffer.alloc(bitmap.getSerializationSizeInBytes(false));
      expect(bitmap.serialize(false, buffer)).eq(buffer);
      expect(RoaringBitmap32.deserialize(buffer, false).toArray()).toEqual(data);
    });

    it("serializes to buffer (portable)", () => {
      const bitmap = new RoaringBitmap32(data);
      const buffer = Buffer.alloc(bitmap.getSerializationSizeInBytes(true));
      expect(bitmap.serialize(true, buffer)).eq(buffer);
      expect(RoaringBitmap32.deserialize(buffer, true).toArray()).toEqual(data);
    });

    it("serializes to buffer (portable), inverted arguments", () => {
      const bitmap = new RoaringBitmap32(data);
      const buffer = Buffer.alloc(bitmap.getSerializationSizeInBytes(true));
      expect(bitmap.serialize(buffer, true)).eq(buffer);
      expect(RoaringBitmap32.deserialize(buffer, true).toArray()).toEqual(data);
    });

    it("handles offset correctly and deserialize correctly", () => {
      const bitmap = new RoaringBitmap32(data);
      const serlength = bitmap.getSerializationSizeInBytes(true);
      const buffer = Buffer.alloc(serlength + 30);
      const serialized = bitmap.serialize("portable", Buffer.from(buffer.buffer, 10));
      expect(Array.from(serialized)).toEqual(Array.from(Buffer.from(buffer.buffer, 10, serlength)));
      expect(RoaringBitmap32.deserialize(serialized, true).toArray()).toEqual(data);
    });
  });

  it("serialize and deserialize empty bitmaps in various formats", async () => {
    for (const format of ["portable", "croaring", "unsafe_frozen_croaring", "uint32_array"] as const) {
      const serialized = new RoaringBitmap32().serialize(format);
      expect(RoaringBitmap32.deserialize(serialized, format).toArray()).toEqual([]);
    }
  });

  it("serialize and deserialize in various formats", async () => {
    for (const format of ["portable", "croaring", "unsafe_frozen_croaring"] as const) {
      const smallArray = [1, 2, 3, 100, 0xfffff, 0xffffffff];
      const serialized = new RoaringBitmap32(smallArray).serialize(format);
      expect(RoaringBitmap32.deserialize(serialized, format).toArray()).toEqual(smallArray);
    }
  });

  it("deserializes text", () => {
    for (const fmt of [
      "comma_separated_values",
      "tab_separated_values",
      "newline_separated_values",
      "json_array",
    ] as const) {
      const bitmap = RoaringBitmap32.deserialize(Buffer.from("1, 2,\n3\t4\n5, 6  ,8 9   10 - 100 -101 102"), fmt);
      expect(bitmap.toArray()).toEqual([1, 2, 3, 4, 5, 6, 8, 9, 10, 100, 102]);
    }
  });
});
