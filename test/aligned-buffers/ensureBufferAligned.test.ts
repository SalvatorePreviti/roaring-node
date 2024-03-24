import { describe, it, expect } from "vitest";
import { RoaringBitmap32, bufferAlignedAlloc, isBufferAligned, ensureBufferAligned } from "../..";

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
