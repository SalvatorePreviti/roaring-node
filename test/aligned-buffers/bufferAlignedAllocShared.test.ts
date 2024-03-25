import { describe, it, expect } from "vitest";
import { RoaringBitmap32, bufferAlignedAllocShared } from "../..";

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
});
