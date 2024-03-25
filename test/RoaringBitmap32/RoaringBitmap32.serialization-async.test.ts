import RoaringBitmap32 from "../../RoaringBitmap32";
import { expect, describe, it } from "vitest";

describe("RoaringBitmap32 serialization", () => {
  const data = [1, 2, 3, 4, 5, 6, 100, 101, 105, 109, 0x7fffffff, 0xfffffffe, 0xffffffff];

  describe("serializeAsync", () => {
    it("serializes to buffer (non portable)", async () => {
      const bitmap = new RoaringBitmap32(data);
      const buffer = Buffer.alloc(bitmap.getSerializationSizeInBytes(false));
      expect(await bitmap.serializeAsync(false, buffer)).eq(buffer);
      expect(RoaringBitmap32.deserialize(buffer, false).toArray()).toEqual(data);
    });

    it("serializes to buffer (portable)", async () => {
      const bitmap = new RoaringBitmap32(data);
      const buffer = Buffer.alloc(bitmap.getSerializationSizeInBytes(true));
      expect(await bitmap.serializeAsync(true, buffer)).eq(buffer);
      expect(RoaringBitmap32.deserialize(buffer, true).toArray()).toEqual(data);
    });

    it("handles offset correctly and deserialize correctly", async () => {
      const bitmap = new RoaringBitmap32(data);
      const serlength = bitmap.getSerializationSizeInBytes(true);
      const buffer = Buffer.alloc(serlength + 30);
      const offsetted = await bitmap.serializeAsync(true, Buffer.from(buffer.buffer, 10));
      expect(Array.from(offsetted)).toEqual(Array.from(Buffer.from(buffer.buffer, 10, serlength)));
      expect(offsetted.buffer).eq(buffer.buffer);
      expect(RoaringBitmap32.deserialize(Buffer.from(buffer.buffer, 10), true).toArray()).toEqual(data);
    });

    it("throws if buffer is not a valid buffer", async () => {
      const bitmap = new RoaringBitmap32(data);
      await expect(bitmap.serializeAsync(false, {} as any)).rejects.toThrow();
      await expect(bitmap.serializeAsync(false, 1 as any)).rejects.toThrow();
      await expect(bitmap.serializeAsync(false, "test" as any)).rejects.toThrow();
      await expect(bitmap.serializeAsync(false, [1, 2, 3] as any)).rejects.toThrow();
    });

    it("throws if buffer is too small", async () => {
      const bitmap = new RoaringBitmap32(data);
      const buffer = Buffer.alloc(bitmap.getSerializationSizeInBytes(false) - 1);
      await expect(bitmap.serializeAsync(false, buffer)).rejects.toThrow();
    });

    it("throws if buffer is too small (portable)", async () => {
      const bitmap = new RoaringBitmap32(data);
      const buffer = Buffer.alloc(bitmap.getSerializationSizeInBytes(true) - 1);
      await expect(bitmap.serializeAsync(true, buffer)).rejects.toThrow();
    });

    it("serializes to buffer (non portable) 1", async () => {
      const bitmap = new RoaringBitmap32(data);
      const buffer = Buffer.alloc(bitmap.getSerializationSizeInBytes(false));
      expect(await bitmap.serializeAsync(false, buffer)).eq(buffer);
      expect(RoaringBitmap32.deserialize(buffer, false).toArray()).toEqual(data);
    });

    it("serializes to buffer (portable) 2", async () => {
      const bitmap = new RoaringBitmap32(data);
      const buffer = Buffer.alloc(bitmap.getSerializationSizeInBytes(true));
      expect(await bitmap.serializeAsync(true, buffer)).eq(buffer);
      expect(RoaringBitmap32.deserialize(buffer, true).toArray()).toEqual(data);
    });

    it("handles offset correctly and deserialize correctly 1", async () => {
      const bitmap = new RoaringBitmap32(data);
      const serlength = bitmap.getSerializationSizeInBytes(true);
      const buffer = Buffer.alloc(serlength + 20);
      const offsetted = await bitmap.serializeAsync(true, Buffer.from(buffer.buffer, 10));
      expect(Array.from(offsetted)).toEqual(Array.from(Buffer.from(buffer.buffer, 10, serlength)));
      expect(offsetted.buffer).eq(buffer.buffer);
      expect(RoaringBitmap32.deserialize(Buffer.from(buffer.buffer, 10), true).toArray()).toEqual(data);
    });
  });

  it("serialize and deserialize empty bitmaps in various formats", async () => {
    for (const format of ["portable", "croaring", "unsafe_frozen_croaring", "uint32_array"] as const) {
      const serialized = await new RoaringBitmap32().serializeAsync(format);
      expect((await RoaringBitmap32.deserializeAsync(serialized, format)).toArray()).toEqual([]);
    }
  });

  it("serialize and deserialize in various formats", async () => {
    for (const format of ["portable", "croaring", "unsafe_frozen_croaring"] as const) {
      const smallArray = [1, 2, 3, 100, 0xfffff, 0xffffffff];
      const serialized = await new RoaringBitmap32(smallArray).serializeAsync(format);
      expect((await RoaringBitmap32.deserializeAsync(serialized, format)).toArray()).toEqual(smallArray);
    }
  });
});
