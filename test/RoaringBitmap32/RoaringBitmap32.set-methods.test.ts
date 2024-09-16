import RoaringBitmap32 from "../../RoaringBitmap32";
import { expect, describe, it } from "vitest";

describe("RoaringBitmap32 comparisons", () => {
  describe("isSubsetOf", () => {
    describe("RoaringBitmap32 isSubsetOf RoaringBitmap32", () => {
      it("returns false for invalid values", () => {
        const bitmap = new RoaringBitmap32();
        expect(bitmap.isSubset(null as any)).eq(false);
        expect(bitmap.isSubset(undefined as any)).eq(false);
        expect(bitmap.isSubset(123 as any)).eq(false);
        expect(bitmap.isSubset([123] as any)).eq(false);
      });

      it("returns true with itself", () => {
        const bitmap = new RoaringBitmap32();
        expect(bitmap.isSubset(bitmap)).eq(true);
      });

      it("returns true with another empty set", () => {
        const a = new RoaringBitmap32();
        const b = new RoaringBitmap32();
        expect(a.isSubset(b)).eq(true);
      });

      it("returns true with another non empty set", () => {
        const a = new RoaringBitmap32();
        const b = new RoaringBitmap32([1, 2, 3]);
        expect(a.isSubset(b)).eq(true);
      });

      it("returns true for bitmap1 < bitmap2", () => {
        const a = new RoaringBitmap32([1, 2]);
        const b = new RoaringBitmap32([1, 2, 3]);
        expect(a.isSubset(b)).eq(true);
      });

      it("returns true for bitmap1 == bitmap2", () => {
        const a = new RoaringBitmap32([1, 2, 3]);
        const b = new RoaringBitmap32([1, 2, 3]);
        expect(a.isSubset(b)).eq(true);
      });

      it("returns true for bitmap1 === bitmap1", () => {
        const bitmap = new RoaringBitmap32([1, 2, 3]);
        expect(bitmap.isSubset(bitmap)).eq(true);
      });

      it("returns false for bitmap1 > bitmap1", () => {
        const a = new RoaringBitmap32([1, 3, 2]);
        const b = new RoaringBitmap32([1, 2]);
        expect(a.isSubset(b)).eq(false);
      });
    });

    describe("RoaringBitmap32 isSubsetOf Set<any>", () => {
      it("returns true with an empty set", () => {
        const bitmap = new RoaringBitmap32();
        expect(bitmap.isSubsetOf(new Set())).eq(true);
      });

      it("returns true with another empty set", () => {
        const a = new RoaringBitmap32();
        const b = new Set();
        expect(a.isSubsetOf(b)).eq(true);
      });

      it("returns true with another non empty set", () => {
        const a = new RoaringBitmap32();
        const b = new Set([1, 2, 3]);
        expect(a.isSubsetOf(b)).eq(true);
      });

      it("returns true for bitmap1 < bitmap2", () => {
        const a = new RoaringBitmap32([1, 2]);
        const b = new Set([1, 2, 3]);
        expect(a.isSubsetOf(b)).eq(true);
      });

      it("returns true for bitmap1 == bitmap2", () => {
        const a = new RoaringBitmap32([1, 2, 3]);
        const b = new Set([1, 2, 3]);
        expect(a.isSubsetOf(b)).eq(true);
      });

      it("returns true for bitmap1 === bitmap1", () => {
        const bitmap = new RoaringBitmap32([1, 2, 3]);
        expect(bitmap.isSubsetOf(bitmap)).eq(true);
      });

      it("returns false for bitmap1 > bitmap2", () => {
        const a = new RoaringBitmap32([1, 3, 2]);
        const b = new Set([1, 2]);
        expect(a.isSubsetOf(b)).eq(false);
      });
    });

    if ("isSubsetOf" in Set.prototype) {
      describe("Set isSubsetOf RoaringBitmap32", () => {
        it("empty set returns true with an empty set", () => {
          const bitmap = new RoaringBitmap32();
          expect(bitmap.isSubsetOf(new Set())).eq(true);
        });

        it("empty set returns true with another empty set", () => {
          const a = new Set();
          const b = new RoaringBitmap32();
          expect(a.isSubsetOf(b)).eq(true);
        });

        it("empty set returns true with another non empty set", () => {
          const a = new Set();
          const b = new RoaringBitmap32([1, 2, 3]);
          expect(a.isSubsetOf(b)).eq(true);
        });

        it("returns true for bitmap1 < bitmap2", () => {
          const a = new Set([1, 2]);
          const b = new RoaringBitmap32([1, 2, 3]);
          expect(a.isSubsetOf(b)).eq(true);
        });

        it("returns true for bitmap1 == bitmap2", () => {
          const a = new Set([1, 2, 3]);
          const b = new RoaringBitmap32([1, 2, 3]);
          expect(a.isSubsetOf(b)).eq(true);
        });

        it("returns true for bitmap1 === bitmap1", () => {
          const bitmap = new Set([1, 2, 3]);
          expect(bitmap.isSubsetOf(bitmap)).eq(true);
        });

        it("returns false for bitmap1 > bitmap2", () => {
          const a = new Set([1, 3, 2]);
          const b = new RoaringBitmap32([1, 2]);
          expect(a.isSubsetOf(b)).eq(false);
        });
      });
    }
  });

  describe("isSupersetOf", () => {
    describe("RoaringBitmap32 isSupersetOf RoaringBitmap32", () => {
      it("returns true with itself", () => {
        const bitmap = new RoaringBitmap32();
        expect(bitmap.isSupersetOf(bitmap)).eq(true);
      });

      it("returns true with another empty set", () => {
        const a = new RoaringBitmap32();
        const b = new RoaringBitmap32();
        expect(a.isSupersetOf(b)).eq(true);
      });

      it("returns true with another non empty set", () => {
        const a = new RoaringBitmap32([1, 2, 3]);
        const b = new RoaringBitmap32();
        expect(a.isSupersetOf(b)).eq(true);
      });

      it("returns true for bitmap1 > bitmap2", () => {
        const a = new RoaringBitmap32([1, 2, 3]);
        const b = new RoaringBitmap32([1, 2]);
        expect(a.isSupersetOf(b)).eq(true);
      });

      it("returns true for bitmap1 == bitmap2", () => {
        const a = new RoaringBitmap32([1, 2, 3]);
        const b = new RoaringBitmap32([1, 2, 3]);
        expect(a.isSupersetOf(b)).eq(true);
      });

      it("returns true for bitmap1 === bitmap1", () => {
        const bitmap = new RoaringBitmap32([1, 2, 3]);
        expect(bitmap.isSupersetOf(bitmap)).eq(true);
      });

      it("returns false for bitmap1 < bitmap2", () => {
        const a = new RoaringBitmap32([1, 2]);
        const b = new RoaringBitmap32([1, 2, 3]);
        expect(a.isSupersetOf(b)).eq(false);
      });
    });

    describe("RoaringBitmap32 isSupersetOf Set<any>", () => {
      it("returns true with an empty set", () => {
        const bitmap = new RoaringBitmap32();
        expect(bitmap.isSupersetOf(new Set())).eq(true);
      });

      it("returns true with another empty set", () => {
        const a = new RoaringBitmap32();
        const b = new Set();
        expect(a.isSupersetOf(b)).eq(true);
      });

      it("returns true with another non empty set", () => {
        const a = new RoaringBitmap32([1, 2, 3]);
        const b = new Set();
        expect(a.isSupersetOf(b)).eq(true);
      });

      it("returns true for bitmap1 > bitmap2", () => {
        const a = new RoaringBitmap32([1, 2, 3]);
        const b = new Set([1, 2]);
        expect(a.isSupersetOf(b)).eq(true);
      });

      it("returns true for bitmap1 == bitmap2", () => {
        const a = new RoaringBitmap32([1, 2, 3]);
        const b = new Set([1, 2, 3]);
        expect(a.isSupersetOf(b)).eq(true);
      });

      it("returns true for bitmap1 === bitmap1", () => {
        const bitmap = new RoaringBitmap32([1, 2, 3]);
        expect(bitmap.isSupersetOf(bitmap)).eq(true);
      });

      it("returns false for bitmap1 < bitmap2", () => {
        const a = new RoaringBitmap32([1, 2]);
        const b = new Set([1, 2, 3]);
        expect(a.isSupersetOf(b)).eq(false);
      });

      it("returns false for bitmap1 < bitmap2 with string", () => {
        const a = new RoaringBitmap32([1, 2]);
        const b = new Set([1, 2, "x"]);
        expect(a.isSupersetOf(b)).eq(false);
      });
    });

    if ("isSupersetOf" in Set.prototype) {
      describe("Set isSupersetOf RoaringBitmap32", () => {
        it("empty set returns true with an empty set", () => {
          const bitmap = new RoaringBitmap32();
          expect(bitmap.isSupersetOf(new Set())).eq(true);
        });

        it("empty set returns true with another empty set", () => {
          const a = new Set();
          const b = new RoaringBitmap32();
          expect(a.isSupersetOf(b)).eq(true);
        });

        it("empty set and not empty bitmap", () => {
          const a = new Set([1, 2, 3]);
          const b = new RoaringBitmap32();
          expect(a.isSupersetOf(b)).eq(true);
        });

        it("returns true for bitmap1 > bitmap2", () => {
          const a = new Set([1, 2, 3]);
          const b = new RoaringBitmap32([1, 2]);
          expect(a.isSupersetOf(b)).eq(true);
        });

        it("returns true for bitmap1 == bitmap2", () => {
          const a = new Set([1, 2, 3]);
          const b = new RoaringBitmap32([1, 2, 3]);
          expect(a.isSupersetOf(b)).eq(true);
        });

        it("returns true for bitmap1 === bitmap1", () => {
          const bitmap = new Set([1, 2, 3]);
          expect(bitmap.isSupersetOf(bitmap)).eq(true);
        });

        it("returns false for bitmap1 < bitmap2", () => {
          const a = new Set([1, 2]);
          const b = new RoaringBitmap32([1, 2, 3]);
          expect(a.isSupersetOf(b)).eq(false);
        });
      });
    }
  });

  describe("isDisjointFrom", () => {
    describe("RoaringBitmap32 isDisjointFrom RoaringBitmap32", () => {
      it("returns true with itself", () => {
        const bitmap = new RoaringBitmap32();
        expect(bitmap.isDisjointFrom(bitmap)).eq(true);
      });

      it("returns true with another empty set", () => {
        const a = new RoaringBitmap32();
        const b = new RoaringBitmap32();
        expect(a.isDisjointFrom(b)).eq(true);
      });

      it("returns true with another non empty set", () => {
        const a = new RoaringBitmap32([1, 2, 3]);
        const b = new RoaringBitmap32();
        expect(a.isDisjointFrom(b)).eq(true);
      });

      it("returns true for bitmap1 > bitmap2", () => {
        const a = new RoaringBitmap32([1, 2, 3]);
        const b = new RoaringBitmap32([4, 5]);
        expect(a.isDisjointFrom(b)).eq(true);
      });

      it("returns true for bitmap1 == bitmap2", () => {
        const a = new RoaringBitmap32([1, 2, 3]);
        const b = new RoaringBitmap32([1, 2, 3]);
        expect(a.isDisjointFrom(b)).eq(false);
      });

      it("returns true for bitmap1 === bitmap1", () => {
        const bitmap = new RoaringBitmap32([1, 2, 3]);
        expect(bitmap.isDisjointFrom(bitmap)).eq(false);
      });

      it("returns false for bitmap1 < bitmap2", () => {
        const a = new RoaringBitmap32([1, 2]);
        const b = new RoaringBitmap32([1, 2, 3]);
        expect(a.isDisjointFrom(b)).eq(false);
      });
    });

    describe("RoaringBitmap32 isDisjointFrom Set<any>", () => {
      it("returns true with an empty set", () => {
        const bitmap = new RoaringBitmap32();
        expect(bitmap.isDisjointFrom(new Set())).eq(true);
      });

      it("returns true with another empty set", () => {
        const a = new RoaringBitmap32();
        const b = new Set();
        expect(a.isDisjointFrom(b)).eq(true);
      });

      it("returns true with another non empty set", () => {
        const a = new RoaringBitmap32([1, 2, 3]);
        const b = new Set();
        expect(a.isDisjointFrom(b)).eq(true);
      });

      it("returns true for bitmap1 > bitmap2", () => {
        const a = new RoaringBitmap32([1, 2, 3]);
        const b = new Set([4, 5]);
        expect(a.isDisjointFrom(b)).eq(true);
      });

      it("returns true for bitmap1 == bitmap2", () => {
        const a = new RoaringBitmap32([1, 2, 3]);
        const b = new Set([1, 2, 3]);
        expect(a.isDisjointFrom(b)).eq(false);
      });

      it("returns true for bitmap1 === bitmap1", () => {
        const bitmap = new RoaringBitmap32([1, 2, 3]);
        expect(bitmap.isDisjointFrom(bitmap)).eq(false);
      });

      it("returns false for bitmap1 < bitmap2", () => {
        const a = new RoaringBitmap32([1, 2]);
        const b = new Set([1, 2, 3]);
        expect(a.isDisjointFrom(b)).eq(false);
      });

      it("returns false for bitmap1 < bitmap2 with string", () => {
        const a = new RoaringBitmap32([1, 2]);
        const b = new Set([1, 2, "x"]);
        expect(a.isDisjointFrom(b)).eq(false);
      });
    });

    if ("isDisjointFrom" in Set.prototype) {
      describe("Set isDisjointFrom RoaringBitmap32", () => {
        it("empty set returns true with an empty set", () => {
          const bitmap = new RoaringBitmap32();
          expect(bitmap.isDisjointFrom(new Set())).eq(true);
        });

        it("empty set returns true with another empty set", () => {
          const a = new Set();
          const b = new RoaringBitmap32();
          expect(a.isDisjointFrom(b)).eq(true);
        });

        it("empty set and not empty bitmap", () => {
          const a = new Set([1, 2, 3]);
          const b = new RoaringBitmap32();
          expect(a.isDisjointFrom(b)).eq(true);
        });

        it("returns true for bitmap1 > bitmap2", () => {
          const a = new Set([1, 2, 3]);
          const b = new RoaringBitmap32([4, 5]);
          expect(a.isDisjointFrom(b)).eq(true);
        });

        it("returns true for bitmap1 == bitmap2", () => {
          const a = new Set([1, 2, 3]);
          const b = new RoaringBitmap32([1, 2, 3]);
          expect(a.isDisjointFrom(b)).eq(false);
        });

        it("returns true for bitmap1 === bitmap1", () => {
          const bitmap = new Set([1, 2, 3]);
          expect(bitmap.isDisjointFrom(bitmap)).eq(false);
        });

        it("returns false for bitmap1 < bitmap2", () => {
          const a = new Set([1, 2]);
          const b = new RoaringBitmap32([1, 2, 3]);
          expect(a.isDisjointFrom(b)).eq(false);
        });
      });
    }
  });

  describe("union", () => {
    describe("RoaringBitmap32 union RoaringBitmap32", () => {
      it("returns the union of two empty RoaringBitmap32", () => {
        const a = new RoaringBitmap32();
        const b = new RoaringBitmap32();
        const result = a.union(b);
        expect(result).toBeInstanceOf(RoaringBitmap32);
        expect(Array.from(result)).toEqual([]);
      });

      it("returns the union of a non empty RoaringBitmap32 with an empty RoaringBitmap32", () => {
        const a = new RoaringBitmap32([1, 2, 3]);
        const b = new RoaringBitmap32();
        const result = a.union(b);
        expect(result).toBeInstanceOf(RoaringBitmap32);
        expect(Array.from(result)).toEqual([1, 2, 3]);
      });

      it("returns the union of two non empty RoaringBitmap32", () => {
        const a = new RoaringBitmap32([1, 2, 3]);
        const b = new RoaringBitmap32([2, 3, 4]);
        const result = a.union(b);
        expect(result).toBeInstanceOf(RoaringBitmap32);
        expect(Array.from(result)).toEqual([1, 2, 3, 4]);
      });

      it("returns the union of two non empty RoaringBitmap32 with the same values", () => {
        const a = new RoaringBitmap32([1, 2, 3]);
        const b = new RoaringBitmap32([1, 2, 3]);
        const result = a.union(b);
        expect(result).toBeInstanceOf(RoaringBitmap32);
        expect(Array.from(result)).toEqual([1, 2, 3]);
      });
    });

    describe("RoaringBitmap32 union Set<any>", () => {
      it("returns the union of an empty RoaringBitmap32 with an empty Set", () => {
        const a = new RoaringBitmap32();
        const b = new Set();
        const result = a.union(b);
        expect(result).toBeInstanceOf(Set);
        expect(Array.from(result)).toEqual([]);
      });

      it("returns the union of a non empty RoaringBitmap32 with an empty Set", () => {
        const a = new RoaringBitmap32([1, 2, 3]);
        const b = new Set();
        const result = a.union(b);
        expect(result).toBeInstanceOf(Set);
        expect(Array.from(result)).toEqual([1, 2, 3]);
      });

      it("returns the union of two non empty RoaringBitmap32", () => {
        const a = new RoaringBitmap32([1, 2, 3]);
        const b = new Set([2, 3, 4]);
        const result = a.union(b);
        expect(result).toBeInstanceOf(Set);
        expect(Array.from(result)).toEqual([1, 2, 3, 4]);
      });

      it("returns the union of two non empty RoaringBitmap32 with the same values", () => {
        const a = new RoaringBitmap32([1, 2, 3]);
        const b = new Set([1, 2, 3]);
        const result = a.union(b);
        expect(result).toBeInstanceOf(Set);
        expect(Array.from(result)).toEqual([1, 2, 3]);
      });
    });

    if ("union" in Set.prototype) {
      describe("Set union RoaringBitmap32", () => {
        it("returns the union of an empty Set with an empty RoaringBitmap32", () => {
          const a = new Set();
          const b = new RoaringBitmap32();
          const result = a.union(b);
          expect(result).toBeInstanceOf(Set);
          expect(Array.from(result)).toEqual([]);
        });

        it("returns the union of a non empty Set with an empty RoaringBitmap32", () => {
          const a = new Set([1, 2, 3]);
          const b = new RoaringBitmap32();
          const result = a.union(b);
          expect(result).toBeInstanceOf(Set);
          expect(Array.from(result)).toEqual([1, 2, 3]);
        });

        it("returns the union of two non empty Set", () => {
          const a = new Set([1, 2, 3]);
          const b = new RoaringBitmap32([2, 3, 4]);
          const result = a.union(b);
          expect(result).toBeInstanceOf(Set);
          expect(Array.from(result)).toEqual([1, 2, 3, 4]);
        });

        it("returns the union of two non empty Set with the same values", () => {
          const a = new Set([1, 2, 3]);
          const b = new RoaringBitmap32([1, 2, 3]);
          const result = a.union(b);
          expect(result).toBeInstanceOf(Set);
          expect(Array.from(result)).toEqual([1, 2, 3]);
        });
      });
    }
  });

  describe("intersection", () => {
    describe("RoaringBitmap32 intersection RoaringBitmap32", () => {
      it("returns the intersection of two empty RoaringBitmap32", () => {
        const a = new RoaringBitmap32();
        const b = new RoaringBitmap32();
        const result = a.intersection(b);
        expect(result).toBeInstanceOf(RoaringBitmap32);
        expect(Array.from(result)).toEqual([]);
      });

      it("returns the intersection of a non empty RoaringBitmap32 with an empty RoaringBitmap32", () => {
        const a = new RoaringBitmap32([1, 2, 3]);
        const b = new RoaringBitmap32();
        const result = a.intersection(b);
        expect(result).toBeInstanceOf(RoaringBitmap32);
        expect(Array.from(result)).toEqual([]);
      });

      it("returns the intersection of two non empty RoaringBitmap32", () => {
        const a = new RoaringBitmap32([1, 2, 3]);
        const b = new RoaringBitmap32([2, 3, 4]);
        const result = a.intersection(b);
        expect(result).toBeInstanceOf(RoaringBitmap32);
        expect(Array.from(result)).toEqual([2, 3]);
      });

      it("returns the intersection of two non empty RoaringBitmap32 with the same values", () => {
        const a = new RoaringBitmap32([1, 2, 3]);
        const b = new RoaringBitmap32([1, 2, 3]);
        const result = a.intersection(b);
        expect(result).toBeInstanceOf(RoaringBitmap32);
        expect(Array.from(result)).toEqual([1, 2, 3]);
      });
    });

    describe("RoaringBitmap32 intersection Set<any>", () => {
      it("returns the intersection of an empty RoaringBitmap32 with an empty Set", () => {
        const a = new RoaringBitmap32();
        const b = new Set();
        const result = a.intersection(b);
        expect(result).toBeInstanceOf(Set);
        expect(Array.from(result)).toEqual([]);
      });

      it("returns the intersection of a non empty RoaringBitmap32 with an empty Set", () => {
        const a = new RoaringBitmap32([1, 2, 3]);
        const b = new Set();
        const result = a.intersection(b);
        expect(result).toBeInstanceOf(Set);
        expect(Array.from(result)).toEqual([]);
      });

      it("returns the intersection of two non empty RoaringBitmap32", () => {
        const a = new RoaringBitmap32([1, 2, 3]);
        const b = new Set([2, 3, 4]);
        const result = a.intersection(b);
        expect(result).toBeInstanceOf(Set);
        expect(Array.from(result)).toEqual([2, 3]);
      });

      it("returns the intersection of two non empty RoaringBitmap32 with the same values", () => {
        const a = new RoaringBitmap32([1, 2, 3]);
        const b = new Set([1, 2, 3]);
        const result = a.intersection(b);
        expect(result).toBeInstanceOf(Set);
        expect(Array.from(result)).toEqual([1, 2, 3]);
      });
    });

    if ("intersection" in Set.prototype) {
      describe("Set intersection RoaringBitmap32", () => {
        it("returns the intersection of an empty Set with an empty RoaringBitmap32", () => {
          const a = new Set();
          const b = new RoaringBitmap32();
          const result = a.intersection(b);
          expect(result).toBeInstanceOf(Set);
          expect(Array.from(result)).toEqual([]);
        });

        it("returns the intersection of a non empty Set with an empty RoaringBitmap32", () => {
          const a = new Set([1, 2, 3]);
          const b = new RoaringBitmap32();
          const result = a.intersection(b);
          expect(result).toBeInstanceOf(Set);
          expect(Array.from(result)).toEqual([]);
        });

        it("returns the intersection of two non empty Set", () => {
          const a = new Set([1, 2, 3]);
          const b = new RoaringBitmap32([2, 3, 4]);
          const result = a.intersection(b);
          expect(result).toBeInstanceOf(Set);
          expect(Array.from(result)).toEqual([2, 3]);
        });

        it("returns the intersection of two non empty Set with the same values", () => {
          const a = new Set([1, 2, 3]);
          const b = new RoaringBitmap32([1, 2, 3]);
          const result = a.intersection(b);
          expect(result).toBeInstanceOf(Set);
          expect(Array.from(result)).toEqual([1, 2, 3]);
        });
      });
    }
  });

  describe("difference", () => {
    describe("RoaringBitmap32 difference RoaringBitmap32", () => {
      it("returns the difference of two empty RoaringBitmap32", () => {
        const a = new RoaringBitmap32();
        const b = new RoaringBitmap32();
        const result = a.difference(b);
        expect(result).toBeInstanceOf(RoaringBitmap32);
        expect(Array.from(result)).toEqual([]);
      });

      it("returns the difference of a non empty RoaringBitmap32 with an empty RoaringBitmap32", () => {
        const a = new RoaringBitmap32([1, 2, 3]);
        const b = new RoaringBitmap32();
        const result = a.difference(b);
        expect(result).toBeInstanceOf(RoaringBitmap32);
        expect(Array.from(result)).toEqual([1, 2, 3]);
      });

      it("returns the difference of two non empty RoaringBitmap32", () => {
        const a = new RoaringBitmap32([1, 2, 3]);
        const b = new RoaringBitmap32([2, 3, 4]);
        const result = a.difference(b);
        expect(result).toBeInstanceOf(RoaringBitmap32);
        expect(Array.from(result)).toEqual([1]);
      });

      it("returns the difference of two non empty RoaringBitmap32 with the same values", () => {
        const a = new RoaringBitmap32([1, 2, 3]);
        const b = new RoaringBitmap32([1, 2, 3]);
        const result = a.difference(b);
        expect(result).toBeInstanceOf(RoaringBitmap32);
        expect(Array.from(result)).toEqual([]);
      });
    });

    describe("RoaringBitmap32 difference Set<any>", () => {
      it("returns the difference of an empty RoaringBitmap32 with an empty Set", () => {
        const a = new RoaringBitmap32();
        const b = new Set();
        const result = a.difference(b);
        expect(result).toBeInstanceOf(Set);
        expect(Array.from(result)).toEqual([]);
      });

      it("returns the difference of a non empty RoaringBitmap32 with an empty Set", () => {
        const a = new RoaringBitmap32([1, 2, 3]);
        const b = new Set();
        const result = a.difference(b);
        expect(result).toBeInstanceOf(Set);
        expect(Array.from(result)).toEqual([1, 2, 3]);
      });

      it("returns the difference of two non empty RoaringBitmap32", () => {
        const a = new RoaringBitmap32([1, 2, 3]);
        const b = new Set([2, 3, 4]);
        const result = a.difference(b);
        expect(result).toBeInstanceOf(Set);
        expect(Array.from(result)).toEqual([1]);
      });

      it("returns the difference of two non empty RoaringBitmap32 with the same values", () => {
        const a = new RoaringBitmap32([1, 2, 3]);
        const b = new Set([1, 2, 3]);
        const result = a.difference(b);
        expect(result).toBeInstanceOf(Set);
        expect(Array.from(result)).toEqual([]);
      });
    });

    if ("intersection" in Set.prototype) {
      describe("Set difference RoaringBitmap32", () => {
        it("returns the difference of an empty Set with an empty RoaringBitmap32", () => {
          const a = new Set();
          const b = new RoaringBitmap32();
          const result = a.difference(b);
          expect(result).toBeInstanceOf(Set);
          expect(Array.from(result)).toEqual([]);
        });

        it("returns the difference of a non empty Set with an empty RoaringBitmap32", () => {
          const a = new Set([1, 2, 3]);
          const b = new RoaringBitmap32();
          const result = a.difference(b);
          expect(result).toBeInstanceOf(Set);
          expect(Array.from(result)).toEqual([1, 2, 3]);
        });

        it("returns the difference of two non empty Set", () => {
          const a = new Set([1, 2, 3]);
          const b = new RoaringBitmap32([2, 3, 4]);
          const result = a.difference(b);
          expect(result).toBeInstanceOf(Set);
          expect(Array.from(result)).toEqual([1]);
        });

        it("returns the difference of two non empty Set with the same values", () => {
          const a = new Set([1, 2, 3]);
          const b = new RoaringBitmap32([1, 2, 3]);
          const result = a.difference(b);
          expect(result).toBeInstanceOf(Set);
          expect(Array.from(result)).toEqual([]);
        });
      });
    }
  });

  describe("symmetricDifference", () => {
    describe("RoaringBitmap32 symmetricDifference RoaringBitmap32", () => {
      it("returns the symmetricDifference of two empty RoaringBitmap32", () => {
        const a = new RoaringBitmap32();
        const b = new RoaringBitmap32();
        const result = a.symmetricDifference(b);
        expect(result).toBeInstanceOf(RoaringBitmap32);
        expect(Array.from(result)).toEqual([]);
      });

      it("returns the symmetricDifference of a non empty RoaringBitmap32 with an empty RoaringBitmap32", () => {
        const a = new RoaringBitmap32([1, 2, 3]);
        const b = new RoaringBitmap32();
        const result = a.symmetricDifference(b);
        expect(result).toBeInstanceOf(RoaringBitmap32);
        expect(Array.from(result)).toEqual([1, 2, 3]);
      });

      it("returns the symmetricDifference of two non empty RoaringBitmap32", () => {
        const a = new RoaringBitmap32([1, 2, 3]);
        const b = new RoaringBitmap32([2, 3, 4]);
        const result = a.symmetricDifference(b);
        expect(result).toBeInstanceOf(RoaringBitmap32);
        expect(Array.from(result)).toEqual([1, 4]);
      });

      it("returns the symmetricDifference of two non empty RoaringBitmap32 with the same values", () => {
        const a = new RoaringBitmap32([1, 2, 3]);
        const b = new RoaringBitmap32([1, 2, 3]);
        const result = a.symmetricDifference(b);
        expect(result).toBeInstanceOf(RoaringBitmap32);
        expect(Array.from(result)).toEqual([]);
      });
    });

    describe("RoaringBitmap32 symmetricDifference Set<any>", () => {
      it("returns the symmetricDifference of an empty RoaringBitmap32 with an empty Set", () => {
        const a = new RoaringBitmap32();
        const b = new Set();
        const result = a.symmetricDifference(b);
        expect(result).toBeInstanceOf(Set);
        expect(Array.from(result)).toEqual([]);
      });

      it("returns the symmetricDifference of a non empty RoaringBitmap32 with an empty Set", () => {
        const a = new RoaringBitmap32([1, 2, 3]);
        const b = new Set();
        const result = a.symmetricDifference(b);
        expect(result).toBeInstanceOf(Set);
        expect(Array.from(result)).toEqual([1, 2, 3]);
      });

      it("returns the symmetricDifference of two non empty RoaringBitmap32", () => {
        const a = new RoaringBitmap32([1, 2, 3]);
        const b = new Set([2, 3, 4]);
        const result = a.symmetricDifference(b);
        expect(result).toBeInstanceOf(Set);
        expect(Array.from(result)).toEqual([1, 4]);
      });

      it("returns the symmetricDifference of two non empty RoaringBitmap32 with the same values", () => {
        const a = new RoaringBitmap32([1, 2, 3]);
        const b = new Set([1, 2, 3]);
        const result = a.symmetricDifference(b);
        expect(result).toBeInstanceOf(Set);
        expect(Array.from(result)).toEqual([]);
      });
    });

    if ("symmetricDifference" in Set.prototype) {
      describe("Set symmetricDifference RoaringBitmap32", () => {
        it("returns the symmetricDifference of an empty Set with an empty RoaringBitmap32", () => {
          const a = new Set();
          const b = new RoaringBitmap32();
          const result = a.symmetricDifference(b);
          expect(result).toBeInstanceOf(Set);
          expect(Array.from(result)).toEqual([]);
        });

        it("returns the symmetricDifference of a non empty Set with an empty RoaringBitmap32", () => {
          const a = new Set([1, 2, 3]);
          const b = new RoaringBitmap32();
          const result = a.symmetricDifference(b);
          expect(result).toBeInstanceOf(Set);
          expect(Array.from(result)).toEqual([1, 2, 3]);
        });

        it("returns the symmetricDifference of two non empty Set", () => {
          const a = new Set([1, 2, 3]);
          const b = new RoaringBitmap32([2, 3, 4]);
          const result = a.symmetricDifference(b);
          expect(result).toBeInstanceOf(Set);
          expect(Array.from(result)).toEqual([1, 4]);
        });

        it("returns the symmetricDifference of two non empty Set with the same values", () => {
          const a = new Set([1, 2, 3]);
          const b = new RoaringBitmap32([1, 2, 3]);
          const result = a.symmetricDifference(b);
          expect(result).toBeInstanceOf(Set);
          expect(Array.from(result)).toEqual([]);
        });
      });
    }
  });
});
