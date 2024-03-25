import { describe, it, expect } from "vitest";
import { RoaringBitmap32, isBufferAligned, bufferAlignedAllocShared } from "../..";

describe("bufferAlignedAllocShared", () => {
  it("exposes the bufferAlignedAllocShared function", () => {
    // eslint-disable-next-line no-console
    console.log("exposes the bufferAlignedAllocShared function");
    expect(bufferAlignedAllocShared).toBeInstanceOf(Function);
    expect(RoaringBitmap32.bufferAlignedAllocShared).eq(bufferAlignedAllocShared);
  });

  it("throws if first argument (size) is invalid", () => {
    // eslint-disable-next-line no-console
    console.log("throws if first argument (size) is invalid");
    expect(() => bufferAlignedAllocShared(-1)).toThrow();
    expect(() => bufferAlignedAllocShared(null as any)).toThrow();
    expect(() => bufferAlignedAllocShared("x" as any)).toThrow();
    expect(() => bufferAlignedAllocShared({} as any)).toThrow();
  });

  it("can allocate an empty buffer", () => {
    // eslint-disable-next-line no-console
    console.log("can allocate an empty buffer");
    const buffer = bufferAlignedAllocShared(0, 512);
    expect(buffer).toBeInstanceOf(Uint8Array);
    expect(buffer.buffer).toBeInstanceOf(SharedArrayBuffer);
    expect(buffer.length).eq(0);
    expect(isBufferAligned(buffer, 512)).eq(true);
  });

  it.only("can allocate a buffer of a given size", () => {
    // eslint-disable-next-line no-console
    console.log("can allocate a buffer of a given size");
    const buffer = bufferAlignedAllocShared(10);
    // eslint-disable-next-line no-console
    console.log("after alloc");
    expect(buffer).toBeInstanceOf(Uint8Array);
    expect(buffer.buffer).toBeInstanceOf(SharedArrayBuffer);
    expect(buffer.length).eq(10);
    expect(isBufferAligned(buffer)).eq(true);
    expect(RoaringBitmap32.isBufferAligned(buffer, 32)).eq(true);
    expect(RoaringBitmap32.isBufferAligned(buffer.buffer)).eq(true);
    // eslint-disable-next-line no-console
    console.log("after checks");
  });
});
