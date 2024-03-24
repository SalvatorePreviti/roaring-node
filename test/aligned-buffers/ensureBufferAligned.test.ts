import {
  RoaringBitmap32,
  bufferAlignedAlloc,
  isBufferAligned,
  ensureBufferAligned,
  bufferAlignedAllocShared,
  bufferAlignedAllocSharedUnsafe,
} from "../..";
import { describe, it, expect } from "vitest";

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
