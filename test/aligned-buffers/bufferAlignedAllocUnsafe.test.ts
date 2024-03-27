import { describe, it, expect } from "vitest";
import { RoaringBitmap32, bufferAlignedAllocUnsafe, isBufferAligned } from "../..";

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
