const roaring = require('./')

/*const iter1 = new roaring.RoaringBitmap32Iterator()
console.log(iter1)

const bitmap = new roaring.RoaringBitmap32()
const iter2 = new roaring.RoaringBitmap32Iterator(bitmap)
console.log(iter2)

console.log(iter2.bitmap)
iter2.bitmap = null
console.log(iter2.bitmap)
*/

for (const v of roaring.RoaringBitmap32([1, 2, 3, 4])) {
  console.log(v)
}

const iter = roaring.RoaringBitmap32Iterator(roaring.RoaringBitmap32([1, 2, 3, 4]))
console.log(iter)
console.log(iter.next())
console.log(iter.next())
console.log(iter.next())
console.log(iter.next())
console.log(iter.next())
console.log(iter.next())

console.log(iter[Symbol.iterator]() === iter)
