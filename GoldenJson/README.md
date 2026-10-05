
- ref : https://twiki.cern.ch/twiki/bin/view/CMSPublic/SWGuideGoodLumiSectionsJSONFile#How_to_understand_the_text_of_Go

- Command
```
cp /afs/cern.ch/cms/CAF/CMSCOMM/COMM_DQM/certification/Collisions18/13TeV/Legacy_2018/Cert_314472-325175_13TeV_Legacy2018_Collisions18_JSON.txt .
cp /afs/cern.ch/cms/CAF/CMSCOMM/COMM_DQM/certification/Collisions17/13TeV/Legacy_2017/Cert_294927-306462_13TeV_UL2017_Collisions17_GoldenJSON.txt .
cp /afs/cern.ch/cms/CAF/CMSCOMM/COMM_DQM/certification/Collisions16/13TeV/Legacy_2016/Cert_271036-284044_13TeV_Legacy2016_Collisions16_JSON.txt .
```

- 2024 (STEP 24, 2026-10-05): `2024_Summer24/Cert_Collisions2024_378981_386951_Golden.json`, the **2026-08-04** version
  (46,348 byte, md5 `3f8543e8062915c9de97472e7dcf744f`, run 475, LS 287,601; docs/reference/LUMI_SOURCES.md section 6.1).
  The directory name is the jsonpog era key that `EraConfig::yearForCorr("2024")` returns. Copied from lxplus with
  (on the Mac; one login):
```
ssh junghyun@lxplus.cern.ch 'cd /eos/user/c/cmsdqm/www/CAF/certification/Collisions24 && tar cf - Cert_Collisions2024_378981_386951_Golden.json' | tar xf - -C GoldenJson/2024_Summer24
```
  The older version of 2024-12-19 (44,548 byte) is still on EOS as `..._Golden_before_TrkML2026_review.json`: check the md5.
