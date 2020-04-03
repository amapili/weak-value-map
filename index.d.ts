export interface WeakValueMapConfig {
    size?: number
}

export default class WeakValueMap<K, V> {
    constructor(conf?: WeakValueMapConfig)
    get(key: K): V | undefined
    set(key: K, value: V): this
    delete(key: K): this
}
