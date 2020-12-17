/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package tw.idv.kenhu.jsfsample.data.dao;
import javax.persistence.EntityManager;
import javax.persistence.Persistence;

/**
 * 利用這個類別以近似Sinleton pattern的方式提供唯一的EntityManager instance.
 * @author ken
 */
public final class EntityManagerFactory {
    /**
     * 儲存單元名稱.
     */
    private static final String PERSISTENCE_UNIT_NAME = "jsf-sample";
    /**
     * JPA EntityManager的唯一實例.
     */
    private static EntityManager entityManager = null;
    /**
     * 私有的無參數contructor,讓外界無法建立實例.
     */
    private EntityManagerFactory() {

    }
    /**
     * 靜態取值方法,用來取得唯一的EntityManager instance.
     * @return an EntityManager instance
     */
    public static EntityManager getEntityManager() {
        if (entityManager == null) {
            entityManager = Persistence.createEntityManagerFactory(
                    PERSISTENCE_UNIT_NAME).createEntityManager();
        }
        return entityManager;
    }
}
