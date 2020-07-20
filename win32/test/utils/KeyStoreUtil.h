/*
 * MIT License
 * Author: David Tillemans
 */
#ifndef KSMGMNT_KEYSTOREUTIL_H
#define KSMGMNT_KEYSTOREUTIL_H


class KeyStoreUtil {
public:
    KeyStoreUtil(const wchar_t *keystoreName);

    void showKeysOfKeystore();

    void deleteKeyFromKeyStore(const wchar_t *keyId);

    boolean isKeyInKeystore(const wchar_t *keyName);

    ~KeyStoreUtil();

private:
    NCRYPT_PROV_HANDLE cryptoProvider;
};


#endif //KSMGMNT_KEYSTOREUTIL_H
