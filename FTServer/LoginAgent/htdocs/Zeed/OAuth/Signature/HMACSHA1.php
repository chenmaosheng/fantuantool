<?php
/**
 * Zeed Platform Project
 * Based on Zeed Framework & Zend Framework.
 * 
 * BTS - Billing Transaction Service
 * CAS - Central Authentication Service
 * 
 * LICENSE
 * http://www.zeed.com.cn/license/
 * 
 * @category   Zeed
 * @package    Zeed_ChangeMe
 * @subpackage ChangeMe
 * @copyright  Copyright (c) 2010 Zeed Technologies PRC Inc. (http://www.zeed.com.cn)
 * @author     Zeed Team (http://blog.zeed.com.cn)
 * @since      Jul 5, 2010
 * @version    SVN: $Id: HMACSHA1.php 5534 2010-07-06 04:46:57Z woody $
 */

/**
 * The HMAC-SHA1 signature method uses the HMAC-SHA1 signature algorithm as defined in [RFC2104] 
 * where the Signature Base String is the text and the key is the concatenated values (each first 
 * encoded per Parameter Encoding) of the Consumer Secret and Token Secret, separated by an '&' 
 * character (ASCII code 38) even if empty.
 * - Chapter 9.2 ("HMAC-SHA1")
 */
class Zeed_OAuth_Signature_HMACSHA1 extends Zeed_OAuth_Signature_Abstract
{
    function getName()
    {
        return "HMAC-SHA1";
    }
    
    public function buildSignature($request, $consumer, $token)
    {
        $baseString = $request->getSignatureBaseString();
        $request->baseString = $baseString;
        
        $keyParts = array($consumer->secret,($token) ? $token->secret : "");
        
        $keyParts = Zeed_OAuth_Util::urlencode_rfc3986($keyParts);
        $key = implode('&', $keyParts);
        
        return base64_encode(hash_hmac('sha1', $baseString, $key, true));
    }
}

// End ^ Native EOL ^ encoding
