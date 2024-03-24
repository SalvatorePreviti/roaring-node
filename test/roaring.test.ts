import roaring from "..";
import { describe, it, expect } from "vitest";

import RoaringBitmap32 from "../RoaringBitmap32";
import RoaringBitmap32Iterator from "../RoaringBitmap32Iterator";

describe("roaring", () => {
  it("is an object", () => {
    expect(typeof roaring).eq("object");
  });

  it('exports itself with a "default" property', () => {
    const required = require("../");
    expect(!!required).toBe(true);
    expect(required === roaring).toBe(true);
    expect(required === required.default).toBe(true);
  });

  it("has RoaringBitmap32", () => {
    expect(typeof roaring.RoaringBitmap32).eq("function");
    expect(roaring.RoaringBitmap32 === RoaringBitmap32).toBe(true);
  });

  it("has RoaringBitmap32Iterator", () => {
    expect(typeof roaring.RoaringBitmap32Iterator).eq("function");
    expect(roaring.RoaringBitmap32Iterator === RoaringBitmap32Iterator).toBe(true);
  });

  it("has CRoaringVersion", () => {
    expect(typeof roaring.CRoaringVersion).eq("string");
    const values = roaring.CRoaringVersion.split(".");
    expect(values).toHaveLength(3);
    for (let i = 0; i < 3; ++i) {
      expect(Number.isInteger(Number.parseInt(values[i], 10))).eq(true);
    }
  });

  it("has roaring PackageVersion", () => {
    expect(typeof roaring.PackageVersion).eq("string");
    const values = roaring.CRoaringVersion.split(".");
    expect(values).toHaveLength(3);
    for (let i = 0; i < 3; ++i) {
      expect(Number.isInteger(Number.parseInt(values[i], 10))).eq(true);
    }
  });
});
