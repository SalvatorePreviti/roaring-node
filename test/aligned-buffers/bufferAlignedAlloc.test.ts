import { RoaringBitmap32, bufferAlignedAlloc, isBufferAligned } from "../..";
import { describe, it, expect } from "vitest";

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
