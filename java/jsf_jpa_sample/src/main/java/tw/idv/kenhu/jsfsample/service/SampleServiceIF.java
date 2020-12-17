/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package tw.idv.kenhu.jsfsample.service;

import java.util.List;
import tw.idv.kenhu.jsfsample.data.entity.GuestMessage;

/**
 * 範例服務介面.
 * @author ken
 */
public interface SampleServiceIF {
    /**
     * 送出留言.
     * @param message 一則新留言
     */
    void sendMessage(tw.idv.kenhu.jsfsample.data.entity.GuestMessage message);
    /**
     * 載入所有留言.
     * @return a list of GuestMessage instances
     */
    List<GuestMessage> loadAllMessages();
}
