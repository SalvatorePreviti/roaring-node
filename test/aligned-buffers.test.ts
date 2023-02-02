import { RoaringBitmap32, bufferAlignedAlloc, bufferAlignedAllocUnsafe, bufferIsAligned } from "../";
import { expect } from "chai";

describe("bufferAlignedAlloc", () => {
  it("exposes the bufferAlignedAlloc function", () => {
    expect(bufferAlignedAlloc).to.be.a("function");
    expect(RoaringBitmap32.bufferAlignedAlloc).to.eq(bufferAlignedAlloc);
  });

  it("throws if first argument (size) is invalid", () => {
    expect(() => bufferAlignedAlloc(-1)).to.throw();
    expect(() => bufferAlignedAlloc("x" as any)).to.throw();
    expect(() => bufferAlignedAlloc({} as any)).to.throw();
    expect(() => bufferAlignedAlloc(null as any)).to.throw();
  });

  it("can allocate an empty buffer", () => {
    const buffer = bufferAlignedAlloc(0);
    expect(buffer).to.be.instanceOf(Buffer);
    expect(buffer.length).to.eq(0);
    expect(bufferIsAligned(buffer)).to.eq(true);
  });

  it("can allocate a buffer of a given size", () => {
    const buffer = bufferAlignedAlloc(10, 128);
    expect(buffer).to.be.instanceOf(Buffer);
    expect(buffer.length).to.eq(10);
    expect(bufferIsAligned(buffer, 128)).to.eq(true);
  });

  it("is initialized with zeroes", () => {
    const buffer = bufferAlignedAlloc(2340);
    expect(buffer).to.be.instanceOf(Buffer);
    expect(buffer.length).to.eq(2340);
    for (let i = 0; i < 2340; ++i) {
      expect(buffer[i]).to.eq(0);
    }
    expect(bufferIsAligned(buffer)).to.eq(true);
    expect(RoaringBitmap32.bufferIsAligned(buffer, 32)).to.eq(true);
  });
});

describe("bufferAlignedAllocUnsafe", () => {
  it("exposes the bufferAlignedAllocUnsafe function", () => {
    expect(bufferAlignedAllocUnsafe).to.be.a("function");
    expect(RoaringBitmap32.bufferAlignedAllocUnsafe).to.eq(bufferAlignedAllocUnsafe);
  });

  it("throws if first argument (size) is invalid", () => {
    expect(() => bufferAlignedAllocUnsafe(-1)).to.throw();
    expect(() => bufferAlignedAllocUnsafe(null as any)).to.throw();
    expect(() => bufferAlignedAllocUnsafe("x" as any)).to.throw();
    expect(() => bufferAlignedAllocUnsafe({} as any)).to.throw();
  });

  it("can allocate an empty buffer", () => {
    const buffer = bufferAlignedAllocUnsafe(0, 512);
    expect(buffer).to.be.instanceOf(Buffer);
    expect(buffer.length).to.eq(0);
    expect(bufferIsAligned(buffer, 512)).to.eq(true);
  });

  it("can allocate a buffer of a given size", () => {
    const buffer = bufferAlignedAllocUnsafe(10);
    expect(buffer).to.be.instanceOf(Buffer);
    expect(buffer.length).to.eq(10);
    expect(bufferIsAligned(buffer)).to.eq(true);
    expect(RoaringBitmap32.bufferIsAligned(buffer, 32)).to.eq(true);
    expect(RoaringBitmap32.bufferIsAligned(buffer.buffer)).to.eq(true);
    expect(RoaringBitmap32.bufferIsAligned(buffer, 111)).to.eq(false);
  });
});
