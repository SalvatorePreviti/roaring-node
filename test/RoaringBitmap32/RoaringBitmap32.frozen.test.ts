import RoaringBitmap32 from "../../RoaringBitmap32";
import { expect } from "chai";

const ERROR_FROZEN = "This bitmap is frozen and cannot be modified";

describe("RoaringBitmap32 frozen", () => {
  describe("freeze", () => {
    it("set isFrozen to true, return this, can be called multiple times", () => {
      const bitmap = new RoaringBitmap32();
      expect(bitmap.isFrozen).to.be.false;
      expect(bitmap.freeze()).to.equal(bitmap);
      expect(bitmap.isFrozen).to.be.true;

      expect(bitmap.freeze()).to.equal(bitmap);
      expect(bitmap.isFrozen).to.be.true;
    });

    it("throws when calling writable methods", () => {
      const bitmap = new RoaringBitmap32();
      bitmap.freeze();

      expect(() => bitmap.copyFrom([1])).to.throw(ERROR_FROZEN);
      expect(() => bitmap.add(1)).to.throw(ERROR_FROZEN);
      expect(() => bitmap.tryAdd(1)).to.throw(ERROR_FROZEN);
      expect(() => bitmap.addMany([1])).to.throw(ERROR_FROZEN);
      expect(() => bitmap.remove(1)).to.throw(ERROR_FROZEN);
      expect(() => bitmap.removeMany([1])).to.throw(ERROR_FROZEN);
      expect(() => bitmap.delete(1)).to.throw(ERROR_FROZEN);
      expect(() => bitmap.clear()).to.throw(ERROR_FROZEN);
      expect(() => bitmap.orInPlace([1])).to.throw(ERROR_FROZEN);
      expect(() => bitmap.andNotInPlace([1])).to.throw(ERROR_FROZEN);
      expect(() => bitmap.andInPlace([1])).to.throw(ERROR_FROZEN);
      expect(() => bitmap.xorInPlace([1])).to.throw(ERROR_FROZEN);
      expect(() => bitmap.flipRange(0, 10)).to.throw(ERROR_FROZEN);
      expect(() => bitmap.addRange(0, 9)).to.throw(ERROR_FROZEN);
      expect(() => bitmap.removeRange(0, 8)).to.throw(ERROR_FROZEN);

      const bitmap1 = new RoaringBitmap32();
      const bitmap2 = new RoaringBitmap32();
      bitmap1.freeze();
      expect(() => RoaringBitmap32.swap(bitmap1, bitmap2)).to.throw(ERROR_FROZEN);
      expect(() => RoaringBitmap32.swap(bitmap2, bitmap1)).to.throw(ERROR_FROZEN);
    });
  });

  it("Is temporarily frozen while using serializeAsync", async () => {
    const bitmap = new RoaringBitmap32([1, 2, 3]);
    expect(bitmap.isFrozen).to.be.false;
    const promise = bitmap.serializeAsync("croaring");
    expect(bitmap.isFrozen).to.be.true;
    await promise;
    expect(bitmap.isFrozen).to.be.false;
    bitmap.add(4);
  });

  it('keep it frozen after calling "serializeAsync"', async () => {
    const bitmap = new RoaringBitmap32([1, 2, 3]);
    bitmap.freeze();
    expect(bitmap.isFrozen).to.be.true;
    const promise = bitmap.serializeAsync("croaring");
    expect(bitmap.isFrozen).to.be.true;
    await promise;
    expect(bitmap.isFrozen).to.be.true;
    expect(() => bitmap.add(4)).to.throw(ERROR_FROZEN);
  });
});
