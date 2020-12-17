/*
 * Client.java    1.0 2017年5月19日
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

import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStreamReader;
import java.io.OutputStream;
import java.io.OutputStreamWriter;
import java.io.PrintWriter;
import java.net.Socket;
import java.util.stream.Collectors;

import javax.xml.bind.JAXBException;

/**
 * Client implementation to access ckip parser.
 *
 * @version 1.0 2017年5月19日
 * @author ken
 *
 */
public class Client {
    /**
     * IP Address of ckip parser server.
     */
    private String ip;
    /**
     * Port number of ckip parser server.
     */
    private int port;
    /**
     * Account for using ckip parser server.
     */
    private String account;
    /**
     * Password for using ckip parser server.
     */
    private String password;

    /**
     * Default constructor.
     * @param ip ip address of ckip parser
     * @param port port number
     * @param account account
     * @param password password
     */
    public Client(String ip, int port, String account, String password) {
        this.ip = ip;
        this.port = port;
        this.account = account;
        this.password = password;
    }

    /**
     * 將語句送往ckip parser進行處理,並取得回傳的結果.
     * @param sentence 一個語句,ex: 台新金控12月3日將召開股東臨時會進行董監改選
     * @return ckip parser的分析結果, 原本是一份xml文件,我們將它轉成一個Java Bean
     */
    public ParserResult parse(String sentence) {
        Socket s = createSocket();
        try (
                OutputStream os = s.getOutputStream();
                OutputStreamWriter osw = new OutputStreamWriter(s.getOutputStream(), "Big5");
                PrintWriter pw = new PrintWriter(osw);
                InputStreamReader isr = new InputStreamReader(s.getInputStream(), "Big5");
                BufferedReader br = new BufferedReader(isr);
                ) {
            pw.println(ParserUtil.generateXmlRequest(account, password, sentence) + "\n");
            pw.flush();
           return ParserUtil.xmlToResult(br.readLine());
        } catch (IOException | JAXBException e) {
           throw new IllegalStateException(e);
        }
    }

    /**
     * Create a tcp socket.
     * @return a tcp socket
     */
    protected Socket createSocket() {
        Socket s = new Socket();
        try {
            s = new Socket(this.ip, this.port);
        } catch (IOException e) {
            throw new IllegalStateException(e);
        }
        return s;
    }
}
