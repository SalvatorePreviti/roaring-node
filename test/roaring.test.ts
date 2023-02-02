import roaring, { DeserializationFormat, FrozenViewFormat, SerializationFormat } from "..";
import { expect } from "chai";

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

  describe("SerializationFormat", () => {
    it("should have the right values", () => {
      expect(SerializationFormat.croaring).eq("croaring");
      expect(SerializationFormat.portable).eq("portable");
      expect(SerializationFormat.frozen_croaring).eq("frozen_croaring");

      expect(Object.values(SerializationFormat)).to.deep.eq(["croaring", "portable", "frozen_croaring"]);

      expect(RoaringBitmap32.SerializationFormat).to.eq(SerializationFormat);

      expect(new RoaringBitmap32().SerializationFormat).to.eq(SerializationFormat);
    });
  });

  describe("DeserializationFormat", () => {
    it("should have the right values", () => {
      expect(DeserializationFormat.croaring).eq("croaring");
      expect(DeserializationFormat.portable).eq("portable");
      expect(DeserializationFormat.frozen_croaring).eq("frozen_croaring");
      expect(DeserializationFormat.frozen_portable).eq("frozen_portable");

      expect(Object.values(DeserializationFormat)).to.deep.eq([
        "croaring",
        "portable",
        "frozen_croaring",
        "frozen_portable",
      ]);

      expect(RoaringBitmap32.DeserializationFormat).to.eq(DeserializationFormat);

      expect(new RoaringBitmap32().DeserializationFormat).to.eq(DeserializationFormat);
    });
  });

  describe("FrozenViewFormat", () => {
    it("should have the right values", () => {
      expect(FrozenViewFormat.frozen_croaring).eq("frozen_croaring");
      expect(FrozenViewFormat.frozen_portable).eq("frozen_portable");

      expect(Object.values(FrozenViewFormat)).to.deep.eq(["frozen_croaring", "frozen_portable"]);

      expect(RoaringBitmap32.FrozenViewFormat).to.eq(FrozenViewFormat);

      expect(new RoaringBitmap32().FrozenViewFormat).to.eq(FrozenViewFormat);
    });
  });
});
