const roaring = require('./')

/*const iterator = new roaring.RoaringBitmap32Iterator()
console.log(iterator)
console.log(iterator.next())*/

const iterator = new roaring.RoaringBitmap32Iterator(new roaring.RoaringBitmap32([1, 2, 3, 4]))

//console.log(iterator)

console.log(iterator.next())
console.log(iterator.next())
console.log(iterator.next())
console.log(iterator.next())
console.log(iterator.next())
console.log(iterator.next())
//console.log(iterator.next())
//console.log(iterator.next())
//console.log(iterator.next())
