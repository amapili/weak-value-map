'use strict';

var WeakValueMap = require('./');
var test = require('tape');

test('map', function(assert) {
	let map = new WeakValueMap();
	map.set(1, 'one')
	   .set(2, 2)
	   .set(3, true)
	   .set(4, false);
	let d1 = new Date();
	let d2 = new Date(d1);
	map.set(5, d1);
	map.set(6, {6: 'six'});
	assert.strictEqual(map.get(1), 'one');
	assert.strictEqual(map.get(2), 2);
	assert.strictEqual(map.get(3), true);
	assert.strictEqual(map.get(4), false);
	assert.strictEqual(map.get(5).getTime(), d2.getTime());
	assert.deepEqual(map.get(6), {6: 'six'});
	assert.strictEqual(map.size, 6);
	map.set(1, 'changed');
	assert.strictEqual(map.get(1), 'changed');
	assert.strictEqual(map.size, 6);
	map.delete(1);
	assert.strictEqual(map.get(1), undefined);
	assert.strictEqual(map.size, 5);
	assert.end();
});

test('gc', function(assert) {
	let map = new WeakValueMap();
	let obj = {a: 1234, b: 'test', c: {d: 'testing'}};
	let obj1 = Object.assign({}, obj);
	let obj2 = Object.assign({}, obj);
	map.set(1, obj1);
	map.set(2, obj2);
	assert.deepEqual(map.get(1), obj);
	assert.deepEqual(map.get(2), obj);
	eval(""); gc();
	assert.deepEqual(map.get(1), obj);
	assert.deepEqual(map.get(2), obj);
	assert.strictEqual(map.size, 2);
	obj2 = null;
	eval(""); gc();
	assert.deepEqual(map.get(1), obj);
	assert.strictEqual(map.get(2), undefined);
	assert.strictEqual(map.size, 1);
	obj1 = null;
	eval(""); gc();
	assert.strictEqual(map.get(1), undefined);
	assert.strictEqual(map.get(2), undefined);
	assert.strictEqual(map.size, 0);
	assert.end();
});

test('exception', function(assert) {
	let map = new WeakValueMap();
	let key = { toString() { throw 42 } };
	let obj = {};
	let err;
	try {
		map.set(key, obj);
	} catch( e ) {
		err = e;
	}
	assert.strictEqual(err, 42);
	assert.strictEqual(map.size, 0);
	assert.end();
});
