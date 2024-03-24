import RoaringBitmap32ReverseIterator from "../../RoaringBitmap32ReverseIterator";
import { describe, it, expect } from "vitest";

describe("RoaringBitmap32ReverseIterator import", () => {
  it('exports itself with a "default" property', () => {
    const required = require("../../RoaringBitmap32ReverseIterator");
    expect(!!required).toBe(true);
    expect(required === required.default).toBe(true);
  });

  it("supports typescript \"import RoaringBitmap32ReverseIterator from 'roaring/RoaringBitmap32ReverseIterator'\" syntax", () => {
    expect(RoaringBitmap32ReverseIterator === require("../../RoaringBitmap32ReverseIterator")).eq(true);
  });

  it("is a class", () => {
    expect(typeof RoaringBitmap32ReverseIterator).eq("function");
    expect(RoaringBitmap32ReverseIterator.prototype.constructor).eq(RoaringBitmap32ReverseIterator);
  });
});
