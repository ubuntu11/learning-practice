/*
 * Result.java    1.0 2017年5月19日
 *
 * Copyright (c) 2017-2030 Monmouth Technologies, Inc.
 * http://www.mt.com.tw
 * 10F-1 No. 306 Chung-Cheng 1st Road, Linya District, 802, Kaoshiung, Taiwan
 * All Rights Reserved.
 *
 * This software is the confidential and proprietary information of Monmouth
 * Technologies, Inc. You shall not disclose such Confidential Information and
 * shall use it only in accordance with the terms of the license agreement you
 * entered into with Monmouth Technologies.
 */
package tw.com.mt.ckipparser;

import javax.xml.bind.annotation.XmlAccessorType;
import javax.xml.bind.annotation.XmlElement;
import javax.xml.bind.annotation.XmlRootElement;
import javax.xml.bind.annotation.XmlAccessType;
import org.eclipse.persistence.oxm.annotations.XmlPath;

/**
 * Class to store ckip parser generated result.
 *
 * @version 1.0 2017年5月19日
 * @author ken
 *
 */
@XmlRootElement(name = "wordsegmentation")
@XmlAccessorType(XmlAccessType.FIELD)
public class ParserResult {
    /**
     * Process status, ex:Success, Service internal error, etc.
     */
    @XmlElement(name = "processstatus")
    private String processStatus;
    /**
     * Process status code, ex: 0, 1, 2, 3.
     */
    @XmlPath("processstatus/@code")
    private int statusCode = -1;
    /**
     * @return the statusCode
     */
    public final int getStatusCode() {
        return statusCode;
    }

    /**
     * @param statusCode the statusCode to set
     */
    public final void setStatusCode(int statusCode) {
        this.statusCode = statusCode;
    }

    /**
     * Generated text including terms, pos and thematic roles.
     */
    @XmlPath("result/sentence/text()")
    private String[] sentence;
    /**
     * @return the processStatus
     */
    public final String getProcessStatus() {
        return processStatus;
    }

    /**
     * @param processStatus the processStatus to set
     */
    public final void setProcessStatus(String processStatus) {
        this.processStatus = processStatus;
    }

    /**
     * @return the sentence
     */
    public final String[] getSentence() {
        return sentence;
    }
    /**
     * @param sentence the sentence to set
     */
    public final void setSentence(String[] sentence) {
        this.sentence = sentence;
    }
}
