/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package tw.idv.kenhu.jsfsample.data.dao;

/**
 * 通用的Dao服務介面,供其它特定類型Entity的Dao介面繼承.
 * @author ken
 * @param <K> Key
 * @param <E> Entity
 */
public interface Dao<K, E> {
    /**
     * 儲存至Database.
     * @param entity 某種Entity instance
     */
    void persist(E entity);
    /**
     * 刪除一個Entity.
     * @param entity 某種Entity instance
     */
    void remove(E entity);
    /**
     * 根據id(主鍵)取得一個Entity.
     * @param id Entity id(主鍵)
     * @return an Entity instance
     */
    E findById(K id);
}
