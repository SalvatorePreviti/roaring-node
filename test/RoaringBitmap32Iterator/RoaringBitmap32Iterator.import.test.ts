import { expect } from "chai";
import RoaringBitmap32Iterator from "../../RoaringBitmap32Iterator";

describe("RoaringBitmap32Iterator import", () => {
  it('exports itself with a "default" property', () => {
    const required = require("../../RoaringBitmap32Iterator");
    expect(!!required).to.be.true;
    expect(required === required.default).to.be.true;
  });

  it("supports typescript \"import RoaringBitmap32Iterator from 'roaring/RoaringBitmap32Iterator'\" syntax", () => {
    expect(RoaringBitmap32Iterator === require("../../RoaringBitmap32Iterator")).eq(true);
  });

  it("is a class", () => {
    expect(typeof RoaringBitmap32Iterator).eq("function");
    expect(RoaringBitmap32Iterator.prototype.constructor).eq(RoaringBitmap32Iterator);
  });
});
