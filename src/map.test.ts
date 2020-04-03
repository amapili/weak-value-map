import WeakValueMap from '../index'

declare function gc(): void

test('map', () => {
	const map = new WeakValueMap<number, any>()
	map.set(1, 'one')
		.set(2, 2)
		.set(3, true)
		.set(4, false)
	const d1 = new Date(),
		d2 = new Date(d1)
	map.set(5, d1)
	map.set(6, { 6: 'six' })
	expect(map.get(1)).toBe('one')
	expect(map.get(2)).toBe(2)
	expect(map.get(3)).toBe(true)
	expect(map.get(4)).toBe(false)
	expect(map.get(5)?.getTime()).toBe(d2.getTime())
	expect(map.get(6)).toEqual({ 6: 'six' })
	map.set(1, 'changed')
	expect(map.get(1)).toBe('changed')
	map.delete(1)
	expect(map.get(1)).toBe(undefined)
})

test('gc', () => {
	const map = new WeakValueMap(),
		obj: any = { a: 1234, b: 'test', c: { d: 'testing' } }
	let obj1 = Object.assign({}, obj),
		obj2 = Object.assign({}, obj)
	map.set(1, obj1)
	map.set(2, obj2)
	expect(map.get(1)).toEqual(obj)
	expect(map.get(2)).toEqual(obj)
	eval(''); gc()
	expect(map.get(1)).toEqual(obj)
	expect(map.get(2)).toEqual(obj)
	obj2 = null
	eval(''); gc()
	expect(map.get(1)).toEqual(obj)
	expect(map.get(2)).toBe(undefined)
	obj1 = null
	eval(''); gc()
	expect(map.get(1)).toBe(undefined)
	expect(map.get(2)).toBe(undefined)
})

test('memory', () => {
	const map = new WeakValueMap({ size: 2 }),
		obj: any = { a: 1234, b: 'test', c: { d: 'testing' } }
	let obj1 = Object.assign({}, obj),
		obj2 = Object.assign({}, obj),
		obj3 = Object.assign({}, obj),
		obj4 = Object.assign({}, obj),
		obj5 = Object.assign({}, obj)
	map.set(1, obj1)
	map.set(2, obj2)
	map.set(3, obj3)
	map.set(4, obj4)
	map.set(5, obj5)
	expect(map.get(1)).toBe(undefined)
	expect(map.get(2)).toBe(undefined)
	expect(map.get(3)).toEqual(obj)
	expect(map.get(4)).toEqual(obj)
	eval(''); gc()
	expect(map.get(3)).toEqual(obj)
	expect(map.get(4)).toEqual(obj)
	obj3 = null
	eval(''); gc()
	expect(map.get(3)).toBe(undefined)
	expect(map.get(4)).toEqual(obj)
	obj4 = null
	eval(''); gc()
	expect(map.get(3)).toBe(undefined)
	expect(map.get(4)).toBe(undefined)
})
