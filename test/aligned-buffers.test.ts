import {
  asBuffer,
  RoaringBitmap32,
  bufferAlignedAlloc,
  bufferAlignedAllocUnsafe,
  isBufferAligned,
  ensureBufferAligned,
  bufferAlignedAllocShared,
  bufferAlignedAllocSharedUnsafe,
} from "..";
import { describe, it, expect } from "vitest";

describe("asBuffer", () => {
  it("wraps an ArrayBuffer", () => {
    const input = new ArrayBuffer(3);
    const output = asBuffer(input);
    expect(output).toBeInstanceOf(Buffer);
    expect(output.buffer).eq(input);
  });

  it("wraps a SharedArrayBuffer", () => {
    const input = new SharedArrayBuffer(3);
    const output = asBuffer(input);
    expect(output).toBeInstanceOf(Buffer);
    expect(output.buffer).eq(input);
  });

  it("wraps an arraybuffer view", () => {
    for (const ctor of [
      Uint8Array,
      Uint16Array,
      Uint32Array,
      Int8Array,
      Int16Array,
      Int32Array,
      Float32Array,
      Float64Array,
      Uint8ClampedArray,
    ]) {
      const input = new ctor(3);
      const output = asBuffer(input);
      expect(output).toBeInstanceOf(Buffer);
      expect(output.buffer).eq(input.buffer);
      expect(output.byteOffset).eq(0);
      expect(output.byteLength).eq(input.byteLength);
    }
  });

  it("wraps an arraybuffer view with offset", () => {
    for (const ctor of [
      Uint8Array,
      Uint16Array,
      Uint32Array,
      Int8Array,
      Int16Array,
      Int32Array,
      Float32Array,
      Float64Array,
      Uint8ClampedArray,
    ]) {
      const sourceArrayBuffer = new ArrayBuffer(20 * ctor.BYTES_PER_ELEMENT);
      const input = new ctor(sourceArrayBuffer, 8, 8);
      const output = asBuffer(input);
      expect(output).toBeInstanceOf(Buffer);
      expect(output.buffer).eq(input.buffer);
      expect(output.byteOffset).eq(input.byteOffset);
      expect(output.byteLength).eq(input.byteLength);
    }
  });
});

describe("bufferAlignedAlloc", () => {
  it("exposes the bufferAlignedAlloc function", () => {
    expect(bufferAlignedAlloc).toBeInstanceOf(Function);
    expect(RoaringBitmap32.bufferAlignedAlloc).eq(bufferAlignedAlloc);
  });

  it("throws if first argument (size) is invalid", () => {
    expect(() => bufferAlignedAlloc(-1)).toThrow();
    expect(() => bufferAlignedAlloc("x" as any)).toThrow();
    expect(() => bufferAlignedAlloc({} as any)).toThrow();
    expect(() => bufferAlignedAlloc(null as any)).toThrow();
  });

  it("can allocate an empty buffer", () => {
    const buffer = bufferAlignedAlloc(0);
    expect(buffer).toBeInstanceOf(Buffer);
    expect(buffer.buffer).toBeInstanceOf(ArrayBuffer);
    expect(buffer.length).eq(0);
    expect(isBufferAligned(buffer)).eq(true);
  });

  it("can allocate a buffer of a given size", () => {
    const buffer = bufferAlignedAlloc(10, 128);
    expect(buffer).toBeInstanceOf(Buffer);
    expect(buffer.buffer).toBeInstanceOf(ArrayBuffer);
    expect(buffer.length).eq(10);
    expect(isBufferAligned(buffer, 128)).eq(true);
  });

  it("is initialized with zeroes", () => {
    const buffer = bufferAlignedAlloc(2340);
    expect(buffer).toBeInstanceOf(Buffer);
    expect(buffer.buffer).toBeInstanceOf(ArrayBuffer);
    expect(buffer.length).eq(2340);
    for (let i = 0; i < 2340; ++i) {
      expect(buffer[i]).eq(0);
    }
    expect(isBufferAligned(buffer)).eq(true);
    expect(RoaringBitmap32.isBufferAligned(buffer, 32)).eq(true);
  });
});

describe("bufferAlignedAllocUnsafe", () => {
  it("exposes the bufferAlignedAllocUnsafe function", () => {
    expect(bufferAlignedAllocUnsafe).toBeInstanceOf(Function);
    expect(RoaringBitmap32.bufferAlignedAllocUnsafe).eq(bufferAlignedAllocUnsafe);
  });

  it("throws if first argument (size) is invalid", () => {
    expect(() => bufferAlignedAllocUnsafe(-1)).toThrow();
    expect(() => bufferAlignedAllocUnsafe(null as any)).toThrow();
    expect(() => bufferAlignedAllocUnsafe("x" as any)).toThrow();
    expect(() => bufferAlignedAllocUnsafe({} as any)).toThrow();
  });

  it("can allocate an empty buffer", () => {
    const buffer = bufferAlignedAllocUnsafe(0, 512);
    expect(buffer).toBeInstanceOf(Buffer);
    expect(buffer.length).eq(0);
    expect(isBufferAligned(buffer, 512)).eq(true);
  });

  it("can allocate a buffer of a given size", () => {
    const buffer = bufferAlignedAllocUnsafe(10);
    expect(buffer).toBeInstanceOf(Buffer);
    expect(buffer.length).eq(10);
    expect(isBufferAligned(buffer)).eq(true);
    expect(RoaringBitmap32.isBufferAligned(buffer, 32)).eq(true);
    expect(RoaringBitmap32.isBufferAligned(buffer.buffer)).eq(true);
  });
});

describe("ensureBufferAligned", () => {
  it("exposes the ensureBufferAligned function", () => {
    expect(ensureBufferAligned).toBeInstanceOf(Function);
    expect(RoaringBitmap32.ensureBufferAligned).eq(ensureBufferAligned);
  });

  it("returns the same buffer if it is already aligned", () => {
    const buffer = bufferAlignedAlloc(30);
    expect(ensureBufferAligned(buffer, 32)).eq(buffer);
  });

  it("returns a new buffer if the buffer is not aligned", () => {
    const unalignedBuffer = Buffer.from(bufferAlignedAlloc(31).buffer, 1, 27);
    const result = ensureBufferAligned(unalignedBuffer);
    expect(result).not.eq(unalignedBuffer);
    expect(result).toBeInstanceOf(Buffer);
    expect(result.length).eq(27);
    expect(isBufferAligned(result)).eq(true);
  });

  it("returns a new buffer if the buffer is not aligned, with a custom alignment", () => {
    const unalignedBuffer = Buffer.from(bufferAlignedAlloc(31).buffer, 1, 30);
    const result = ensureBufferAligned(unalignedBuffer, 256);
    expect(result).not.eq(unalignedBuffer);
    expect(result).toBeInstanceOf(Buffer);
    expect(result.length).eq(30);
    expect(isBufferAligned(result, 256)).eq(true);
  });

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

const nodeVersion = parseInt(process.versions.node.split(".")[0], 10);
if (nodeVersion >= 12) {
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
      expect(buffer).toBeInstanceOf(Buffer);
      expect(buffer.buffer).toBeInstanceOf(SharedArrayBuffer);
      expect(buffer.length).eq(0);
      expect(isBufferAligned(buffer, 512)).eq(true);
    });

    it("can allocate a buffer of a given size", () => {
      const buffer = bufferAlignedAllocShared(10);
      expect(buffer).toBeInstanceOf(Buffer);
      expect(buffer.buffer).toBeInstanceOf(SharedArrayBuffer);
      expect(buffer.length).eq(10);
      expect(isBufferAligned(buffer)).eq(true);
      expect(RoaringBitmap32.isBufferAligned(buffer, 32)).eq(true);
      expect(RoaringBitmap32.isBufferAligned(buffer.buffer)).eq(true);
    });
  });

  describe("bufferAlignedAllocSharedUnsafe", () => {
    it("exposes the bufferAlignedAllocSharedUnsafe function", () => {
      expect(bufferAlignedAllocSharedUnsafe).toBeInstanceOf(Function);
      expect(RoaringBitmap32.bufferAlignedAllocSharedUnsafe).eq(bufferAlignedAllocSharedUnsafe);
    });

    it("throws if first argument (size) is invalid", () => {
      expect(() => bufferAlignedAllocSharedUnsafe(-1)).toThrow();
      expect(() => bufferAlignedAllocSharedUnsafe(null as any)).toThrow();
      expect(() => bufferAlignedAllocSharedUnsafe("x" as any)).toThrow();
      expect(() => bufferAlignedAllocSharedUnsafe({} as any)).toThrow();
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
      expect(RoaringBitmap32.isBufferAligned(buffer.buffer)).eq(true);
    });
  });
}
