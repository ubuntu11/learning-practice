/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package tw.idv.kenhu.jsfsample.service;

import java.util.List;
import javax.faces.bean.ApplicationScoped;
import javax.faces.bean.ManagedBean;
import javax.faces.bean.ManagedProperty;
import javax.persistence.EntityTransaction;
import org.apache.log4j.Logger;
import tw.idv.kenhu.jsfsample.data.dao.EntityManagerFactory;
import tw.idv.kenhu.jsfsample.data.dao.UserDaoIF;
import tw.idv.kenhu.jsfsample.data.entity.GuestMessage;

/**
 * 服務介面的實作類別.
 * @author ken
 */
@ManagedBean(name = "sampleService")
@ApplicationScoped
public class SampleService implements SampleServiceIF {
    /**
     * Log4j logger instance.
     */
    private final Logger logger = Logger.getLogger(SampleService.class);

    /**
     * @param userDao the userDao to set
     */
    public void setUserDao(UserDaoIF userDao) {
        this.userDao = userDao;
    }
    /**
     * UserDoaIF實作instance.
     */
    @ManagedProperty(value = "#{userDao}")
    private UserDaoIF userDao;

    @Override
    public void sendMessage(GuestMessage message) {
        logger.info("receive a new message: " + message.toString());
        EntityTransaction tx = EntityManagerFactory.getEntityManager().getTransaction();
        tx.begin();
        userDao.persist(message);
        tx.commit();
    }

    @Override
    public List<GuestMessage> loadAllMessages() {
        return userDao.loadAllMessages();
    }

}
