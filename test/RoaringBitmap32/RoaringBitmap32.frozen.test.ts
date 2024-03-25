import { FrozenViewFormat } from "../..";
import RoaringBitmap32 from "../../RoaringBitmap32";
import { expect, describe, it } from "vitest";

const ERROR_FROZEN = "This bitmap is frozen and cannot be modified";

describe("RoaringBitmap32 frozen", () => {
  describe("FrozenViewFormat", () => {
    it("should have the right values", () => {
      expect(FrozenViewFormat.unsafe_frozen_croaring).eq("unsafe_frozen_croaring");
      expect(FrozenViewFormat.unsafe_frozen_portable).eq("unsafe_frozen_portable");

      expect(Object.values(FrozenViewFormat)).toEqual(["unsafe_frozen_croaring", "unsafe_frozen_portable"]);

      expect(RoaringBitmap32.FrozenViewFormat).eq(FrozenViewFormat);

      expect(new RoaringBitmap32().FrozenViewFormat).eq(FrozenViewFormat);
    });
  });

  describe("freeze", () => {
    it("set isFrozen to true, return this, can be called multiple times", () => {
      const bitmap = new RoaringBitmap32();
      expect(bitmap.isFrozen).toBe(false);
      expect(bitmap.freeze()).equal(bitmap);
      expect(bitmap.isFrozen).toBe(true);

      expect(bitmap.freeze()).equal(bitmap);
      expect(bitmap.isFrozen).toBe(true);
    });

    it("throws when calling writable methods", () => {
      const bitmap = new RoaringBitmap32();
      bitmap.freeze();

      expect(() => bitmap.copyFrom([1])).toThrow(ERROR_FROZEN);
      expect(() => bitmap.add(1)).toThrow(ERROR_FROZEN);
      expect(() => bitmap.tryAdd(1)).toThrow(ERROR_FROZEN);
      expect(() => bitmap.addMany([1])).toThrow(ERROR_FROZEN);
      expect(() => bitmap.remove(1)).toThrow(ERROR_FROZEN);
      expect(() => bitmap.removeMany([1])).toThrow(ERROR_FROZEN);
      expect(() => bitmap.delete(1)).toThrow(ERROR_FROZEN);
      expect(() => bitmap.clear()).toThrow(ERROR_FROZEN);
      expect(() => bitmap.orInPlace([1])).toThrow(ERROR_FROZEN);
      expect(() => bitmap.andNotInPlace([1])).toThrow(ERROR_FROZEN);
      expect(() => bitmap.andInPlace([1])).toThrow(ERROR_FROZEN);
      expect(() => bitmap.xorInPlace([1])).toThrow(ERROR_FROZEN);
      expect(() => bitmap.flipRange(0, 10)).toThrow(ERROR_FROZEN);
      expect(() => bitmap.addRange(0, 9)).toThrow(ERROR_FROZEN);
      expect(() => bitmap.removeRange(0, 8)).toThrow(ERROR_FROZEN);

      const bitmap1 = new RoaringBitmap32();
      const bitmap2 = new RoaringBitmap32();
      bitmap1.freeze();
      expect(() => RoaringBitmap32.swap(bitmap1, bitmap2)).toThrow(ERROR_FROZEN);
      expect(() => RoaringBitmap32.swap(bitmap2, bitmap1)).toThrow(ERROR_FROZEN);
    });
  });

  describe("serializeAsync", () => {
    it("Is temporarily frozen while using serializeAsync", async () => {
      const bitmap = new RoaringBitmap32([1, 2, 3]);
      expect(bitmap.isFrozen).toBe(false);
      const promise = bitmap.serializeAsync("croaring");
      expect(bitmap.isFrozen).toBe(true);
      await promise;
      expect(bitmap.isFrozen).toBe(false);
      bitmap.add(4);
    });

    it('keep it frozen after calling "serializeAsync"', async () => {
      const bitmap = new RoaringBitmap32([1, 2, 3]);
      bitmap.freeze();
      expect(bitmap.isFrozen).toBe(true);
      const promise = bitmap.serializeAsync("croaring");
      expect(bitmap.isFrozen).toBe(true);
      await promise;
      expect(bitmap.isFrozen).toBe(true);
      expect(() => bitmap.add(4)).toThrow(ERROR_FROZEN);
    });
  });

  describe("unsafeFrozenView", () => {
    it("throws if the buffer is an invalid type", () => {
      expect(() => RoaringBitmap32.unsafeFrozenView({} as any, "unsafe_frozen_croaring")).toThrow();
      expect(() => RoaringBitmap32.unsafeFrozenView([] as any, "unsafe_frozen_croaring")).toThrow();
      expect(() => RoaringBitmap32.unsafeFrozenView(null as any, "unsafe_frozen_croaring")).toThrow();
      expect(() => RoaringBitmap32.unsafeFrozenView(undefined as any, "unsafe_frozen_croaring")).toThrow();
      expect(() => RoaringBitmap32.unsafeFrozenView(new Uint32Array(1), "unsafe_frozen_croaring")).toThrow();
    });

    it("throws if format is invalid", () => {
      expect(() => RoaringBitmap32.unsafeFrozenView(new Uint8Array(10), "a" as any)).toThrow();
      expect(() => RoaringBitmap32.unsafeFrozenView(new Uint8Array(10), "" as any)).toThrow();
      expect(() => RoaringBitmap32.unsafeFrozenView(new Uint8Array(10), undefined as any)).toThrow();
      expect(() => RoaringBitmap32.unsafeFrozenView(new Uint8Array(10), null as any)).toThrow();
    });

    it("can create a view from a serialized bitmap, and the view is frozen", () => {
      const values = [1, 2, 3, 100, 8772837, 0x7ffffff1, 0x7fffffff];
      const bitmap = new RoaringBitmap32(values);
      const serialized = bitmap.serialize("unsafe_frozen_croaring");
      expect(RoaringBitmap32.isBufferAligned(serialized)).toBe(true);
      const view = RoaringBitmap32.unsafeFrozenView(serialized, "unsafe_frozen_croaring");
      expect(view.isFrozen).toBe(true);
      expect(view.toArray()).toEqual(values);
      expect(() => view.add(4)).toThrow(ERROR_FROZEN);
      expect(() => view.runOptimize()).toThrow(ERROR_FROZEN);
      expect(() => view.shrinkToFit()).toThrow(ERROR_FROZEN);
      expect(() => view.removeRunCompression()).toThrow(ERROR_FROZEN);

      const copy = view.clone();
      expect(copy.isFrozen).toBe(false);
      expect(copy.toArray()).toEqual(values);
      expect(copy.tryAdd(4)).toBe(true);
    });

    it("can create a view from a serialized bitmap in a SharedArrayBuffer, and the view is frozen", () => {
      const values = [1, 2, 3, 100, 8772837, 0x7ffffff1, 0x7fffffff];
      const bitmap = new RoaringBitmap32(values);
      const sharedBuffer = RoaringBitmap32.bufferAlignedAllocShared(
        bitmap.getSerializationSizeInBytes("unsafe_frozen_croaring"),
      );
      expect(sharedBuffer.buffer).toBeInstanceOf(SharedArrayBuffer);
      const serialized = bitmap.serialize("unsafe_frozen_croaring", sharedBuffer);
      expect(serialized).eq(sharedBuffer);

      const view = RoaringBitmap32.unsafeFrozenView(sharedBuffer, "unsafe_frozen_croaring");
      expect(view.isFrozen).toBe(true);
      expect(view.toArray()).toEqual(values);

      const copy = view.clone();
      expect(copy.isFrozen).toBe(false);
      expect(copy.toArray()).toEqual(values);
      expect(copy.tryAdd(4)).toBe(true);
    });
  });

  describe("asReadonlyView", () => {
    it("offers a readonly view", () => {
      const bitmap = new RoaringBitmap32([1, 2, 3]);
      const readonlyView = bitmap.asReadonlyView() as RoaringBitmap32;
      expect(readonlyView).not.eq(bitmap);
      expect(readonlyView.isFrozen).toBe(true);
      expect(readonlyView.asReadonlyView()).eq(readonlyView);
      expect(bitmap.asReadonlyView()).equal(readonlyView);

      expect(() => readonlyView.copyFrom([1])).toThrow(ERROR_FROZEN);
      expect(() => readonlyView.add(1)).toThrow(ERROR_FROZEN);
      expect(() => readonlyView.tryAdd(1)).toThrow(ERROR_FROZEN);
      expect(() => readonlyView.addMany([1])).toThrow(ERROR_FROZEN);
      expect(() => readonlyView.remove(1)).toThrow(ERROR_FROZEN);
      expect(() => readonlyView.removeMany([1])).toThrow(ERROR_FROZEN);
      expect(() => readonlyView.delete(1)).toThrow(ERROR_FROZEN);
      expect(() => readonlyView.clear()).toThrow(ERROR_FROZEN);
      expect(() => readonlyView.orInPlace([1])).toThrow(ERROR_FROZEN);
      expect(() => readonlyView.andNotInPlace([1])).toThrow(ERROR_FROZEN);
      expect(() => readonlyView.andInPlace([1])).toThrow(ERROR_FROZEN);
      expect(() => readonlyView.xorInPlace([1])).toThrow(ERROR_FROZEN);
      expect(() => readonlyView.flipRange(0, 10)).toThrow(ERROR_FROZEN);
      expect(() => readonlyView.addRange(0, 9)).toThrow(ERROR_FROZEN);
      expect(() => readonlyView.removeRange(0, 8)).toThrow(ERROR_FROZEN);
      expect(() => readonlyView.removeRunCompression()).toThrow(ERROR_FROZEN);
      expect(() => readonlyView.runOptimize()).toThrow(ERROR_FROZEN);
      expect(() => readonlyView.shrinkToFit()).toThrow(ERROR_FROZEN);

      bitmap.add(100);
      expect(readonlyView.toArray()).toEqual([1, 2, 3, 100]);
      expect(readonlyView.size).equal(4);
      bitmap.add(200);
      expect(readonlyView.toArray()).toEqual([1, 2, 3, 100, 200]);
      expect(readonlyView.size).equal(5);
      expect(readonlyView.size).equal(5);

      bitmap.freeze();
      expect(readonlyView.isFrozen).toBe(true);
      expect(bitmap.asReadonlyView()).eq(readonlyView);
    });
  });

  it("returns this if the bitmap is frozen", () => {
    const bitmap = new RoaringBitmap32();
    bitmap.freeze();
    expect(bitmap.asReadonlyView()).equal(bitmap);
  });
});
