import { describe, expect, it } from "vitest";
import RoaringBitmap32ReverseIterator from "../../RoaringBitmap32ReverseIterator";

describe("RoaringBitmap32ReverseIterator import", () => {
  it('exports itself with a "default" property', () => {
    const required = require("../../RoaringBitmap32ReverseIterator");
    expect(!!required).to.be.true;
    expect(required === required.default).to.be.true;
  });

  it("supports typescript \"import RoaringBitmap32ReverseIterator from 'roaring/RoaringBitmap32ReverseIterator'\" syntax", () => {
    expect(RoaringBitmap32ReverseIterator === require("../../RoaringBitmap32ReverseIterator")).eq(true);
  });

  it("is a class", () => {
    expect(typeof RoaringBitmap32ReverseIterator).eq("function");
    expect(RoaringBitmap32ReverseIterator.prototype.constructor).eq(RoaringBitmap32ReverseIterator);
  });
});
