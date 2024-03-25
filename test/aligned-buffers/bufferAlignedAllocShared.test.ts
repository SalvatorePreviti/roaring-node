import { describe, it, expect } from "vitest";
import { RoaringBitmap32, isBufferAligned, bufferAlignedAllocShared } from "../..";

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
