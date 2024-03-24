import { describe, it, expect } from "vitest";
import { RoaringBitmap32, bufferAlignedAllocSharedUnsafe, isBufferAligned } from "../..";

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
    expect(RoaringBitmap32.isBufferAligned(buffer.buffer)).eq(true);
  });
});
