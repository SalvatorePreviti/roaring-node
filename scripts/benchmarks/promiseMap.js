function promiseMap(collection, functor, concurrency) {
  return new Promise((resolve, reject) => {
    let running = 0;
    let index = 0;
    let rejected = false;
    const iterator = collection[Symbol.iterator]();

    function doNext() {
      if (rejected) {
        return false;
      }

      const current = iterator.next();
      if (current.done) {
        if (running === 0 && !rejected) {
          resolve();
        }
        return false;
      }

      Promise.resolve(functor(current.value, index)).then(onResolved).catch(onRejected);
      index++;
      running++;
      return true;
    }

    function onRejected(reason) {
      rejected = true;
      reject(reason);
    }

    function onResolved() {
      running--;
      doNext();
    }

    for (;;) {
      if (!(running < concurrency && doNext())) {
        break;
      }
    }
  });
}

module.exports = promiseMap;
