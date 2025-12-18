import { expect } from "chai";
import roaring from "..";

import RoaringBitmap32 from "../RoaringBitmap32";
import RoaringBitmap32Iterator from "../RoaringBitmap32Iterator";

describe("roaring", () => {
  it("is an object", () => {
    expect(typeof roaring).eq("object");
  });

  it('exports itself with a "default" property', () => {
    const required = require("../");
    expect(!!required).to.be.true;
    expect(required === roaring).to.be.true;
    expect(required === required.default).to.be.true;
  });

  it("has RoaringBitmap32", () => {
    expect(typeof roaring.RoaringBitmap32).eq("function");
    expect(roaring.RoaringBitmap32 === RoaringBitmap32).to.be.true;
  });

  it("has RoaringBitmap32Iterator", () => {
    expect(typeof roaring.RoaringBitmap32Iterator).eq("function");
    expect(roaring.RoaringBitmap32Iterator === RoaringBitmap32Iterator).to.be.true;
  });

  it("has CRoaringVersion", () => {
    expect(typeof roaring.CRoaringVersion).eq("string");
    const values = roaring.CRoaringVersion.split(".");
    expect(values).to.have.lengthOf(3);
    for (let i = 0; i < 3; ++i) {
      expect(Number.isInteger(Number.parseInt(values[i], 10))).eq(true);
    }
  });

  it("has roaring PackageVersion", () => {
    expect(typeof roaring.PackageVersion).eq("string");
    const values = roaring.CRoaringVersion.split(".");
    expect(values).to.have.lengthOf(3);
    for (let i = 0; i < 3; ++i) {
      expect(Number.isInteger(Number.parseInt(values[i], 10))).eq(true);
    }
  });
});
