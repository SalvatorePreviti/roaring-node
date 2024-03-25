import RoaringBitmap32Iterator from "../../RoaringBitmap32Iterator";
import { describe, it, expect } from "vitest";

describe("RoaringBitmap32Iterator import", () => {
  it('exports itself with a "default" property', () => {
    const required = require("../../RoaringBitmap32Iterator");
    expect(!!required).toBe(true);
    expect(required === required.default).toBe(true);
  });

  it("supports typescript \"import RoaringBitmap32Iterator from 'roaring/RoaringBitmap32Iterator'\" syntax", () => {
    expect(RoaringBitmap32Iterator === require("../../RoaringBitmap32Iterator")).eq(true);
  });

  it("is a class", () => {
    expect(typeof RoaringBitmap32Iterator).eq("function");
    expect(RoaringBitmap32Iterator.prototype.constructor).eq(RoaringBitmap32Iterator);
  });
});
