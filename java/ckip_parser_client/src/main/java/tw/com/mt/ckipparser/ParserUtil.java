/*
 * ParserUtil.java    1.0 2017年5月19日
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

import java.io.ByteArrayInputStream;
import java.io.InputStream;
import java.nio.charset.StandardCharsets;
import javax.xml.bind.JAXBContext;
import javax.xml.bind.JAXBException;
import javax.xml.bind.Unmarshaller;
import org.eclipse.persistence.jaxb.JAXBContextFactory;
import com.c05mic.generictree.Tree;
/**
 * Utility class for using ckip parser.
 *
 * @version 1.0 2017年5月19日
 * @author ken
 *
 */
public class ParserUtil {
    /**
     * String format used to generate xml request for ckip parser.
     */
    private static final String XML_REQ_FORMAT = "<?xml version=\"1.0\" ?>"
            + "<wordsegmentation version=\"0.1\"><option showcategory=\"1\" />"
            + "<authentication username=\"%s\" password=\"%s\" /><text>%s</text>"
            + "</wordsegmentation>";
    /**
     * Convert ckip parser generated result(as xml format) to a java bean.
     * @param xml ckip parser generated result
     * @return a ParserResult instance
     * @throws JAXBException if anything goes wrong during xml data binding process
     */
    public static ParserResult xmlToResult(final String xml) throws JAXBException {
        ParserResult result = new ParserResult();
        InputStream stream = new ByteArrayInputStream(xml.getBytes(StandardCharsets.UTF_8));
        JAXBContext jaxbContext = JAXBContextFactory
                .createContext(new Class[] {ParserResult.class}, null);
        Unmarshaller jaxbUnmarshaller = jaxbContext.createUnmarshaller();
        result = (ParserResult) jaxbUnmarshaller.unmarshal(stream);
        return result;
    }

    /**
     * Generate request sending to ckip parser as xml.
     * @param account ckip parser account
     * @param password ckip parser password
     * @param sentence original utf-8 encoded sentence needs to be parsed by ckip parser
     * @return an xml string
     */
    public static String generateXmlRequest(String account, String password, String sentence) {
        String s = String.format(XML_REQ_FORMAT, account, password, sentence);
        return s;
    }

    /**
     * 把ckip parser產生的分析結果字串(ex: #1:1.[0] S(NP(N:手機|Head:Nv:上網)|ADV:如何|Head:Vt:設定)
     * #?(QUESTIONCATEGORY))轉換成Term的樹狀結構.
     * @param text ckip parser產生的分析結果字串
     * @return Term的樹狀結構
     */
    public static Tree<Term> taggedTextToTermTree(String text) {
        TermTreeConverter converter = new TermTreeConverter(text);
        return converter.convert();
    }
}
