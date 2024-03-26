import { describe, it, expect } from "vitest";
import {
  RoaringBitmap32,
  asBuffer,
  isBufferAligned,
  bufferAlignedAllocShared,
  bufferAlignedAllocSharedUnsafe,
  ensureBufferAligned,
} from "..";

describe("shared arrays", () => {
  describe("asBuffer", () => {
    it("wraps a SharedArrayBuffer", () => {
      const input = new SharedArrayBuffer(3);
      const output = asBuffer(input);
      expect(output).toBeInstanceOf(Buffer);
      expect(output.buffer).eq(input);
    });
  });

  describe("bufferAlignedAllocShared", () => {
    it("exposes the bufferAlignedAllocShared function", () => {
      expect(bufferAlignedAllocShared).toBeInstanceOf(Function);
      expect(RoaringBitmap32.bufferAlignedAllocShared).eq(bufferAlignedAllocShared);
    });

    it("throws if first argument (size) is invalid", () => {
      expect(() => bufferAlignedAllocShared(-1)).toThrow();
      expect(() => bufferAlignedAllocShared(null as any)).toThrow();
      expect(() => bufferAlignedAllocShared("x" as any)).toThrow();
      expect(() => bufferAlignedAllocShared({} as any)).toThrow();
    });

    it("can allocate an empty buffer", () => {
      const buffer = bufferAlignedAllocShared(0, 512);
      expect(buffer).toBeInstanceOf(Uint8Array);
      expect(buffer.buffer).toBeInstanceOf(SharedArrayBuffer);
      expect(buffer.length).eq(0);
      expect(isBufferAligned(buffer, 512)).eq(true);
    });

    it.only("can allocate a buffer of a given size", () => {
      const buffer = bufferAlignedAllocShared(10);
      expect(buffer).toBeInstanceOf(Uint8Array);
      expect(buffer.buffer).toBeInstanceOf(SharedArrayBuffer);
      expect(buffer.length).eq(10);
      expect(isBufferAligned(buffer)).eq(true);
      expect(RoaringBitmap32.isBufferAligned(buffer, 32)).eq(true);
    });
  });

  describe("bufferAlignedAllocSharedUnsafe", () => {
    it("exposes the bufferAlignedAllocSharedUnsafe function", () => {
      expect(bufferAlignedAllocSharedUnsafe).toBeInstanceOf(Function);
      expect(RoaringBitmap32.bufferAlignedAllocSharedUnsafe).eq(bufferAlignedAllocSharedUnsafe);
    });

    it("throws if first argument (size) is invalid", () => {
      expect(() => bufferAlignedAllocSharedUnsafe(-1)).throw();
      expect(() => bufferAlignedAllocSharedUnsafe(null as any)).throw();
      expect(() => bufferAlignedAllocSharedUnsafe("x" as any)).throw();
      expect(() => bufferAlignedAllocSharedUnsafe({} as any)).throw();
    });

    it("can allocate an empty buffer", () => {
      const buffer = bufferAlignedAllocSharedUnsafe(0, 512);
      expect(buffer).toBeInstanceOf(Buffer);
      expect(buffer.buffer).toBeInstanceOf(SharedArrayBuffer);
      expect(buffer.length).eq(0);
      expect(isBufferAligned(buffer, 512)).eq(true);
    });

    it("can allocate a buffer of a given size", () => {
      const buffer = bufferAlignedAllocSharedUnsafe(10);
      expect(buffer).toBeInstanceOf(Buffer);
      expect(buffer.buffer).toBeInstanceOf(SharedArrayBuffer);
      expect(buffer.length).eq(10);
      expect(isBufferAligned(buffer)).eq(true);
      expect(RoaringBitmap32.isBufferAligned(buffer, 32)).eq(true);
    });
  });

  describe("ensureBufferAligned", () => {
    it("works with SharedArrayBuffer", () => {
      const sab = new SharedArrayBuffer(32);
      const unalignedBuffer = Buffer.from(sab, 1, 30);
      expect(unalignedBuffer.buffer).eq(sab);
      const result = ensureBufferAligned(unalignedBuffer);
      expect(result).not.eq(unalignedBuffer);
      expect(result).toBeInstanceOf(Buffer);
      expect(result.buffer).toBeInstanceOf(SharedArrayBuffer);
      expect(result.length).eq(30);
      expect(isBufferAligned(result)).eq(true);
    });
  });

  describe("RoaringBitmap32", () => {
    describe("rangeUint32Array", () => {
      it("accepts a single argument of SharedArrayBuffer", () => {
        const bitmap = new RoaringBitmap32([1, 2, 10, 30, 50, 70, 100]);
        const output = new SharedArrayBuffer(3 * 4);
        const result = bitmap.rangeUint32Array(output);
        expect(Array.from(result)).toEqual([1, 2, 10]);
        expect(result.buffer).eq(output);
      });
    });

    describe("toUint32Array", () => {
      it("supports SharedArrayBuffer", () => {
        const bitmap = new RoaringBitmap32([1, 2, 10, 30, 0x7fffffff, 0xffffffff]);
        const output = new Uint32Array(new SharedArrayBuffer(6 * 4));
        expect(bitmap.toUint32Array(output)).eq(output);
        expect(Array.from(output)).toEqual([1, 2, 10, 30, 0x7fffffff, 0xffffffff]);
      });
    });

    describe("toUint32ArrayAsync", () => {
      it("writes the bitmap to the output SharedArrayBuffer", async () => {
        const bitmap = new RoaringBitmap32([1, 2, 10, 30, 0x7fffffff, 0xffffffff]);
        const output = new SharedArrayBuffer(6 * 4);
        const result = await bitmap.toUint32ArrayAsync(output);
        expect(result.buffer).eq(output);
        expect(Array.from(result)).toEqual([1, 2, 10, 30, 0x7fffffff, 0xffffffff]);
      });
    });

    describe("RoaringBitmap32 frozen", () => {
      describe("unsafeFrozenView", () => {
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
    });

    describe("RoaringBitmap32 serialization", () => {
      describe("serialize", () => {
        it("can serialize to a SharedArrayBuffer", () => {
          const data = [1, 2, 3, 4, 5, 6, 100, 101, 105, 109, 0x7fffffff, 0xfffffffe, 0xffffffff];
          const bitmap = new RoaringBitmap32(data);
          const output = new SharedArrayBuffer(bitmap.getSerializationSizeInBytes(true));
          const buffer = bitmap.serialize("portable", output);
          expect(buffer.buffer).eq(output);
          expect(Array.from(new Uint8Array(buffer))).toEqual(Array.from(bitmap.serialize(true)));
        });
      });
    });
  });
});
