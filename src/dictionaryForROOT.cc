// Do NOT change. Changes will be lost next time file is generated

#define R__DICTIONARY_FILENAME srcdIdictionaryForROOT
#define R__NO_DEPRECATION

/*******************************************************************/
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#define G__DICTIONARY
#include "ROOT/RConfig.hxx"
#include "TClass.h"
#include "TDictAttributeMap.h"
#include "TInterpreter.h"
#include "TROOT.h"
#include "TBuffer.h"
#include "TMemberInspector.h"
#include "TInterpreter.h"
#include "TVirtualMutex.h"
#include "TError.h"

#ifndef G__ROOT
#define G__ROOT
#endif

#include "RtypesImp.h"
#include "TIsAProxy.h"
#include "TFileMergeInfo.h"
#include <algorithm>
#include "TCollectionProxyInfo.h"
/*******************************************************************/

#include "TDataMember.h"

// Header files passed as explicit arguments
#include "include/tnm.h"

// Header files passed via #pragma extra_include

// The generated code does not explicitly qualify STL entities
namespace std {} using namespace std;

namespace ROOT {
   static void *new_itreestream(void *p = nullptr);
   static void *newArray_itreestream(Long_t size, void *p);
   static void delete_itreestream(void *p);
   static void deleteArray_itreestream(void *p);
   static void destruct_itreestream(void *p);

   // Function generating the singleton type initializer
   static TGenericClassInfo *GenerateInitInstanceLocal(const ::itreestream*)
   {
      ::itreestream *ptr = nullptr;
      static ::TVirtualIsAProxy* isa_proxy = new ::TInstrumentedIsAProxy< ::itreestream >(nullptr);
      static ::ROOT::TGenericClassInfo 
         instance("itreestream", ::itreestream::Class_Version(), "treestream.h", 160,
                  typeid(::itreestream), ::ROOT::Internal::DefineBehavior(ptr, ptr),
                  &::itreestream::Dictionary, isa_proxy, 4,
                  sizeof(::itreestream) );
      instance.SetNew(&new_itreestream);
      instance.SetNewArray(&newArray_itreestream);
      instance.SetDelete(&delete_itreestream);
      instance.SetDeleteArray(&deleteArray_itreestream);
      instance.SetDestructor(&destruct_itreestream);
      return &instance;
   }
   TGenericClassInfo *GenerateInitInstance(const ::itreestream*)
   {
      return GenerateInitInstanceLocal(static_cast<::itreestream*>(nullptr));
   }
   // Static variable to force the class initialization
   static ::ROOT::TGenericClassInfo *_R__UNIQUE_DICT_(Init) = GenerateInitInstanceLocal(static_cast<const ::itreestream*>(nullptr)); R__UseDummy(_R__UNIQUE_DICT_(Init));
} // end of namespace ROOT

namespace ROOT {
   static void *new_otreestream(void *p = nullptr);
   static void *newArray_otreestream(Long_t size, void *p);
   static void delete_otreestream(void *p);
   static void deleteArray_otreestream(void *p);
   static void destruct_otreestream(void *p);

   // Function generating the singleton type initializer
   static TGenericClassInfo *GenerateInitInstanceLocal(const ::otreestream*)
   {
      ::otreestream *ptr = nullptr;
      static ::TVirtualIsAProxy* isa_proxy = new ::TInstrumentedIsAProxy< ::otreestream >(nullptr);
      static ::ROOT::TGenericClassInfo 
         instance("otreestream", ::otreestream::Class_Version(), "treestream.h", 408,
                  typeid(::otreestream), ::ROOT::Internal::DefineBehavior(ptr, ptr),
                  &::otreestream::Dictionary, isa_proxy, 4,
                  sizeof(::otreestream) );
      instance.SetNew(&new_otreestream);
      instance.SetNewArray(&newArray_otreestream);
      instance.SetDelete(&delete_otreestream);
      instance.SetDeleteArray(&deleteArray_otreestream);
      instance.SetDestructor(&destruct_otreestream);
      return &instance;
   }
   TGenericClassInfo *GenerateInitInstance(const ::otreestream*)
   {
      return GenerateInitInstanceLocal(static_cast<::otreestream*>(nullptr));
   }
   // Static variable to force the class initialization
   static ::ROOT::TGenericClassInfo *_R__UNIQUE_DICT_(Init) = GenerateInitInstanceLocal(static_cast<const ::otreestream*>(nullptr)); R__UseDummy(_R__UNIQUE_DICT_(Init));
} // end of namespace ROOT

namespace ROOT {
   static void *new_eventBuffer(void *p = nullptr);
   static void *newArray_eventBuffer(Long_t size, void *p);
   static void delete_eventBuffer(void *p);
   static void deleteArray_eventBuffer(void *p);
   static void destruct_eventBuffer(void *p);

   // Function generating the singleton type initializer
   static TGenericClassInfo *GenerateInitInstanceLocal(const ::eventBuffer*)
   {
      ::eventBuffer *ptr = nullptr;
      static ::TVirtualIsAProxy* isa_proxy = new ::TInstrumentedIsAProxy< ::eventBuffer >(nullptr);
      static ::ROOT::TGenericClassInfo 
         instance("eventBuffer", ::eventBuffer::Class_Version(), "eventBuffer.h", 23,
                  typeid(::eventBuffer), ::ROOT::Internal::DefineBehavior(ptr, ptr),
                  &::eventBuffer::Dictionary, isa_proxy, 4,
                  sizeof(::eventBuffer) );
      instance.SetNew(&new_eventBuffer);
      instance.SetNewArray(&newArray_eventBuffer);
      instance.SetDelete(&delete_eventBuffer);
      instance.SetDeleteArray(&deleteArray_eventBuffer);
      instance.SetDestructor(&destruct_eventBuffer);
      return &instance;
   }
   TGenericClassInfo *GenerateInitInstance(const ::eventBuffer*)
   {
      return GenerateInitInstanceLocal(static_cast<::eventBuffer*>(nullptr));
   }
   // Static variable to force the class initialization
   static ::ROOT::TGenericClassInfo *_R__UNIQUE_DICT_(Init) = GenerateInitInstanceLocal(static_cast<const ::eventBuffer*>(nullptr)); R__UseDummy(_R__UNIQUE_DICT_(Init));
} // end of namespace ROOT

namespace ROOT {
   static TClass *eventBuffercLcLCorrT1METJet_s_Dictionary();
   static void eventBuffercLcLCorrT1METJet_s_TClassManip(TClass*);
   static void *new_eventBuffercLcLCorrT1METJet_s(void *p = nullptr);
   static void *newArray_eventBuffercLcLCorrT1METJet_s(Long_t size, void *p);
   static void delete_eventBuffercLcLCorrT1METJet_s(void *p);
   static void deleteArray_eventBuffercLcLCorrT1METJet_s(void *p);
   static void destruct_eventBuffercLcLCorrT1METJet_s(void *p);

   // Function generating the singleton type initializer
   static TGenericClassInfo *GenerateInitInstanceLocal(const ::eventBuffer::CorrT1METJet_s*)
   {
      ::eventBuffer::CorrT1METJet_s *ptr = nullptr;
      static ::TVirtualIsAProxy* isa_proxy = new ::TIsAProxy(typeid(::eventBuffer::CorrT1METJet_s));
      static ::ROOT::TGenericClassInfo 
         instance("eventBuffer::CorrT1METJet_s", "eventBuffer.h", 1095,
                  typeid(::eventBuffer::CorrT1METJet_s), ::ROOT::Internal::DefineBehavior(ptr, ptr),
                  &eventBuffercLcLCorrT1METJet_s_Dictionary, isa_proxy, 0,
                  sizeof(::eventBuffer::CorrT1METJet_s) );
      instance.SetNew(&new_eventBuffercLcLCorrT1METJet_s);
      instance.SetNewArray(&newArray_eventBuffercLcLCorrT1METJet_s);
      instance.SetDelete(&delete_eventBuffercLcLCorrT1METJet_s);
      instance.SetDeleteArray(&deleteArray_eventBuffercLcLCorrT1METJet_s);
      instance.SetDestructor(&destruct_eventBuffercLcLCorrT1METJet_s);
      return &instance;
   }
   TGenericClassInfo *GenerateInitInstance(const ::eventBuffer::CorrT1METJet_s*)
   {
      return GenerateInitInstanceLocal(static_cast<::eventBuffer::CorrT1METJet_s*>(nullptr));
   }
   // Static variable to force the class initialization
   static ::ROOT::TGenericClassInfo *_R__UNIQUE_DICT_(Init) = GenerateInitInstanceLocal(static_cast<const ::eventBuffer::CorrT1METJet_s*>(nullptr)); R__UseDummy(_R__UNIQUE_DICT_(Init));

   // Dictionary for non-ClassDef classes
   static TClass *eventBuffercLcLCorrT1METJet_s_Dictionary() {
      TClass* theClass =::ROOT::GenerateInitInstanceLocal(static_cast<const ::eventBuffer::CorrT1METJet_s*>(nullptr))->GetClass();
      eventBuffercLcLCorrT1METJet_s_TClassManip(theClass);
   return theClass;
   }

   static void eventBuffercLcLCorrT1METJet_s_TClassManip(TClass* ){
   }

} // end of namespace ROOT

namespace ROOT {
   static TClass *eventBuffercLcLElectron_s_Dictionary();
   static void eventBuffercLcLElectron_s_TClassManip(TClass*);
   static void *new_eventBuffercLcLElectron_s(void *p = nullptr);
   static void *newArray_eventBuffercLcLElectron_s(Long_t size, void *p);
   static void delete_eventBuffercLcLElectron_s(void *p);
   static void deleteArray_eventBuffercLcLElectron_s(void *p);
   static void destruct_eventBuffercLcLElectron_s(void *p);

   // Function generating the singleton type initializer
   static TGenericClassInfo *GenerateInitInstanceLocal(const ::eventBuffer::Electron_s*)
   {
      ::eventBuffer::Electron_s *ptr = nullptr;
      static ::TVirtualIsAProxy* isa_proxy = new ::TIsAProxy(typeid(::eventBuffer::Electron_s));
      static ::ROOT::TGenericClassInfo 
         instance("eventBuffer::Electron_s", "eventBuffer.h", 1116,
                  typeid(::eventBuffer::Electron_s), ::ROOT::Internal::DefineBehavior(ptr, ptr),
                  &eventBuffercLcLElectron_s_Dictionary, isa_proxy, 0,
                  sizeof(::eventBuffer::Electron_s) );
      instance.SetNew(&new_eventBuffercLcLElectron_s);
      instance.SetNewArray(&newArray_eventBuffercLcLElectron_s);
      instance.SetDelete(&delete_eventBuffercLcLElectron_s);
      instance.SetDeleteArray(&deleteArray_eventBuffercLcLElectron_s);
      instance.SetDestructor(&destruct_eventBuffercLcLElectron_s);
      return &instance;
   }
   TGenericClassInfo *GenerateInitInstance(const ::eventBuffer::Electron_s*)
   {
      return GenerateInitInstanceLocal(static_cast<::eventBuffer::Electron_s*>(nullptr));
   }
   // Static variable to force the class initialization
   static ::ROOT::TGenericClassInfo *_R__UNIQUE_DICT_(Init) = GenerateInitInstanceLocal(static_cast<const ::eventBuffer::Electron_s*>(nullptr)); R__UseDummy(_R__UNIQUE_DICT_(Init));

   // Dictionary for non-ClassDef classes
   static TClass *eventBuffercLcLElectron_s_Dictionary() {
      TClass* theClass =::ROOT::GenerateInitInstanceLocal(static_cast<const ::eventBuffer::Electron_s*>(nullptr))->GetClass();
      eventBuffercLcLElectron_s_TClassManip(theClass);
   return theClass;
   }

   static void eventBuffercLcLElectron_s_TClassManip(TClass* ){
   }

} // end of namespace ROOT

namespace ROOT {
   static TClass *eventBuffercLcLFatJet_s_Dictionary();
   static void eventBuffercLcLFatJet_s_TClassManip(TClass*);
   static void *new_eventBuffercLcLFatJet_s(void *p = nullptr);
   static void *newArray_eventBuffercLcLFatJet_s(Long_t size, void *p);
   static void delete_eventBuffercLcLFatJet_s(void *p);
   static void deleteArray_eventBuffercLcLFatJet_s(void *p);
   static void destruct_eventBuffercLcLFatJet_s(void *p);

   // Function generating the singleton type initializer
   static TGenericClassInfo *GenerateInitInstanceLocal(const ::eventBuffer::FatJet_s*)
   {
      ::eventBuffer::FatJet_s *ptr = nullptr;
      static ::TVirtualIsAProxy* isa_proxy = new ::TIsAProxy(typeid(::eventBuffer::FatJet_s));
      static ::ROOT::TGenericClassInfo 
         instance("eventBuffer::FatJet_s", "eventBuffer.h", 1243,
                  typeid(::eventBuffer::FatJet_s), ::ROOT::Internal::DefineBehavior(ptr, ptr),
                  &eventBuffercLcLFatJet_s_Dictionary, isa_proxy, 0,
                  sizeof(::eventBuffer::FatJet_s) );
      instance.SetNew(&new_eventBuffercLcLFatJet_s);
      instance.SetNewArray(&newArray_eventBuffercLcLFatJet_s);
      instance.SetDelete(&delete_eventBuffercLcLFatJet_s);
      instance.SetDeleteArray(&deleteArray_eventBuffercLcLFatJet_s);
      instance.SetDestructor(&destruct_eventBuffercLcLFatJet_s);
      return &instance;
   }
   TGenericClassInfo *GenerateInitInstance(const ::eventBuffer::FatJet_s*)
   {
      return GenerateInitInstanceLocal(static_cast<::eventBuffer::FatJet_s*>(nullptr));
   }
   // Static variable to force the class initialization
   static ::ROOT::TGenericClassInfo *_R__UNIQUE_DICT_(Init) = GenerateInitInstanceLocal(static_cast<const ::eventBuffer::FatJet_s*>(nullptr)); R__UseDummy(_R__UNIQUE_DICT_(Init));

   // Dictionary for non-ClassDef classes
   static TClass *eventBuffercLcLFatJet_s_Dictionary() {
      TClass* theClass =::ROOT::GenerateInitInstanceLocal(static_cast<const ::eventBuffer::FatJet_s*>(nullptr))->GetClass();
      eventBuffercLcLFatJet_s_TClassManip(theClass);
   return theClass;
   }

   static void eventBuffercLcLFatJet_s_TClassManip(TClass* ){
   }

} // end of namespace ROOT

namespace ROOT {
   static TClass *eventBuffercLcLFsrPhoton_s_Dictionary();
   static void eventBuffercLcLFsrPhoton_s_TClassManip(TClass*);
   static void *new_eventBuffercLcLFsrPhoton_s(void *p = nullptr);
   static void *newArray_eventBuffercLcLFsrPhoton_s(Long_t size, void *p);
   static void delete_eventBuffercLcLFsrPhoton_s(void *p);
   static void deleteArray_eventBuffercLcLFsrPhoton_s(void *p);
   static void destruct_eventBuffercLcLFsrPhoton_s(void *p);

   // Function generating the singleton type initializer
   static TGenericClassInfo *GenerateInitInstanceLocal(const ::eventBuffer::FsrPhoton_s*)
   {
      ::eventBuffer::FsrPhoton_s *ptr = nullptr;
      static ::TVirtualIsAProxy* isa_proxy = new ::TIsAProxy(typeid(::eventBuffer::FsrPhoton_s));
      static ::ROOT::TGenericClassInfo 
         instance("eventBuffer::FsrPhoton_s", "eventBuffer.h", 1450,
                  typeid(::eventBuffer::FsrPhoton_s), ::ROOT::Internal::DefineBehavior(ptr, ptr),
                  &eventBuffercLcLFsrPhoton_s_Dictionary, isa_proxy, 0,
                  sizeof(::eventBuffer::FsrPhoton_s) );
      instance.SetNew(&new_eventBuffercLcLFsrPhoton_s);
      instance.SetNewArray(&newArray_eventBuffercLcLFsrPhoton_s);
      instance.SetDelete(&delete_eventBuffercLcLFsrPhoton_s);
      instance.SetDeleteArray(&deleteArray_eventBuffercLcLFsrPhoton_s);
      instance.SetDestructor(&destruct_eventBuffercLcLFsrPhoton_s);
      return &instance;
   }
   TGenericClassInfo *GenerateInitInstance(const ::eventBuffer::FsrPhoton_s*)
   {
      return GenerateInitInstanceLocal(static_cast<::eventBuffer::FsrPhoton_s*>(nullptr));
   }
   // Static variable to force the class initialization
   static ::ROOT::TGenericClassInfo *_R__UNIQUE_DICT_(Init) = GenerateInitInstanceLocal(static_cast<const ::eventBuffer::FsrPhoton_s*>(nullptr)); R__UseDummy(_R__UNIQUE_DICT_(Init));

   // Dictionary for non-ClassDef classes
   static TClass *eventBuffercLcLFsrPhoton_s_Dictionary() {
      TClass* theClass =::ROOT::GenerateInitInstanceLocal(static_cast<const ::eventBuffer::FsrPhoton_s*>(nullptr))->GetClass();
      eventBuffercLcLFsrPhoton_s_TClassManip(theClass);
   return theClass;
   }

   static void eventBuffercLcLFsrPhoton_s_TClassManip(TClass* ){
   }

} // end of namespace ROOT

namespace ROOT {
   static TClass *eventBuffercLcLGenIsolatedPhoton_s_Dictionary();
   static void eventBuffercLcLGenIsolatedPhoton_s_TClassManip(TClass*);
   static void *new_eventBuffercLcLGenIsolatedPhoton_s(void *p = nullptr);
   static void *newArray_eventBuffercLcLGenIsolatedPhoton_s(Long_t size, void *p);
   static void delete_eventBuffercLcLGenIsolatedPhoton_s(void *p);
   static void deleteArray_eventBuffercLcLGenIsolatedPhoton_s(void *p);
   static void destruct_eventBuffercLcLGenIsolatedPhoton_s(void *p);

   // Function generating the singleton type initializer
   static TGenericClassInfo *GenerateInitInstanceLocal(const ::eventBuffer::GenIsolatedPhoton_s*)
   {
      ::eventBuffer::GenIsolatedPhoton_s *ptr = nullptr;
      static ::TVirtualIsAProxy* isa_proxy = new ::TIsAProxy(typeid(::eventBuffer::GenIsolatedPhoton_s));
      static ::ROOT::TGenericClassInfo 
         instance("eventBuffer::GenIsolatedPhoton_s", "eventBuffer.h", 1473,
                  typeid(::eventBuffer::GenIsolatedPhoton_s), ::ROOT::Internal::DefineBehavior(ptr, ptr),
                  &eventBuffercLcLGenIsolatedPhoton_s_Dictionary, isa_proxy, 0,
                  sizeof(::eventBuffer::GenIsolatedPhoton_s) );
      instance.SetNew(&new_eventBuffercLcLGenIsolatedPhoton_s);
      instance.SetNewArray(&newArray_eventBuffercLcLGenIsolatedPhoton_s);
      instance.SetDelete(&delete_eventBuffercLcLGenIsolatedPhoton_s);
      instance.SetDeleteArray(&deleteArray_eventBuffercLcLGenIsolatedPhoton_s);
      instance.SetDestructor(&destruct_eventBuffercLcLGenIsolatedPhoton_s);
      return &instance;
   }
   TGenericClassInfo *GenerateInitInstance(const ::eventBuffer::GenIsolatedPhoton_s*)
   {
      return GenerateInitInstanceLocal(static_cast<::eventBuffer::GenIsolatedPhoton_s*>(nullptr));
   }
   // Static variable to force the class initialization
   static ::ROOT::TGenericClassInfo *_R__UNIQUE_DICT_(Init) = GenerateInitInstanceLocal(static_cast<const ::eventBuffer::GenIsolatedPhoton_s*>(nullptr)); R__UseDummy(_R__UNIQUE_DICT_(Init));

   // Dictionary for non-ClassDef classes
   static TClass *eventBuffercLcLGenIsolatedPhoton_s_Dictionary() {
      TClass* theClass =::ROOT::GenerateInitInstanceLocal(static_cast<const ::eventBuffer::GenIsolatedPhoton_s*>(nullptr))->GetClass();
      eventBuffercLcLGenIsolatedPhoton_s_TClassManip(theClass);
   return theClass;
   }

   static void eventBuffercLcLGenIsolatedPhoton_s_TClassManip(TClass* ){
   }

} // end of namespace ROOT

namespace ROOT {
   static TClass *eventBuffercLcLGenJet_s_Dictionary();
   static void eventBuffercLcLGenJet_s_TClassManip(TClass*);
   static void *new_eventBuffercLcLGenJet_s(void *p = nullptr);
   static void *newArray_eventBuffercLcLGenJet_s(Long_t size, void *p);
   static void delete_eventBuffercLcLGenJet_s(void *p);
   static void deleteArray_eventBuffercLcLGenJet_s(void *p);
   static void destruct_eventBuffercLcLGenJet_s(void *p);

   // Function generating the singleton type initializer
   static TGenericClassInfo *GenerateInitInstanceLocal(const ::eventBuffer::GenJet_s*)
   {
      ::eventBuffer::GenJet_s *ptr = nullptr;
      static ::TVirtualIsAProxy* isa_proxy = new ::TIsAProxy(typeid(::eventBuffer::GenJet_s));
      static ::ROOT::TGenericClassInfo 
         instance("eventBuffer::GenJet_s", "eventBuffer.h", 1492,
                  typeid(::eventBuffer::GenJet_s), ::ROOT::Internal::DefineBehavior(ptr, ptr),
                  &eventBuffercLcLGenJet_s_Dictionary, isa_proxy, 0,
                  sizeof(::eventBuffer::GenJet_s) );
      instance.SetNew(&new_eventBuffercLcLGenJet_s);
      instance.SetNewArray(&newArray_eventBuffercLcLGenJet_s);
      instance.SetDelete(&delete_eventBuffercLcLGenJet_s);
      instance.SetDeleteArray(&deleteArray_eventBuffercLcLGenJet_s);
      instance.SetDestructor(&destruct_eventBuffercLcLGenJet_s);
      return &instance;
   }
   TGenericClassInfo *GenerateInitInstance(const ::eventBuffer::GenJet_s*)
   {
      return GenerateInitInstanceLocal(static_cast<::eventBuffer::GenJet_s*>(nullptr));
   }
   // Static variable to force the class initialization
   static ::ROOT::TGenericClassInfo *_R__UNIQUE_DICT_(Init) = GenerateInitInstanceLocal(static_cast<const ::eventBuffer::GenJet_s*>(nullptr)); R__UseDummy(_R__UNIQUE_DICT_(Init));

   // Dictionary for non-ClassDef classes
   static TClass *eventBuffercLcLGenJet_s_Dictionary() {
      TClass* theClass =::ROOT::GenerateInitInstanceLocal(static_cast<const ::eventBuffer::GenJet_s*>(nullptr))->GetClass();
      eventBuffercLcLGenJet_s_TClassManip(theClass);
   return theClass;
   }

   static void eventBuffercLcLGenJet_s_TClassManip(TClass* ){
   }

} // end of namespace ROOT

namespace ROOT {
   static TClass *eventBuffercLcLGenJetAK8_s_Dictionary();
   static void eventBuffercLcLGenJetAK8_s_TClassManip(TClass*);
   static void *new_eventBuffercLcLGenJetAK8_s(void *p = nullptr);
   static void *newArray_eventBuffercLcLGenJetAK8_s(Long_t size, void *p);
   static void delete_eventBuffercLcLGenJetAK8_s(void *p);
   static void deleteArray_eventBuffercLcLGenJetAK8_s(void *p);
   static void destruct_eventBuffercLcLGenJetAK8_s(void *p);

   // Function generating the singleton type initializer
   static TGenericClassInfo *GenerateInitInstanceLocal(const ::eventBuffer::GenJetAK8_s*)
   {
      ::eventBuffer::GenJetAK8_s *ptr = nullptr;
      static ::TVirtualIsAProxy* isa_proxy = new ::TIsAProxy(typeid(::eventBuffer::GenJetAK8_s));
      static ::ROOT::TGenericClassInfo 
         instance("eventBuffer::GenJetAK8_s", "eventBuffer.h", 1515,
                  typeid(::eventBuffer::GenJetAK8_s), ::ROOT::Internal::DefineBehavior(ptr, ptr),
                  &eventBuffercLcLGenJetAK8_s_Dictionary, isa_proxy, 0,
                  sizeof(::eventBuffer::GenJetAK8_s) );
      instance.SetNew(&new_eventBuffercLcLGenJetAK8_s);
      instance.SetNewArray(&newArray_eventBuffercLcLGenJetAK8_s);
      instance.SetDelete(&delete_eventBuffercLcLGenJetAK8_s);
      instance.SetDeleteArray(&deleteArray_eventBuffercLcLGenJetAK8_s);
      instance.SetDestructor(&destruct_eventBuffercLcLGenJetAK8_s);
      return &instance;
   }
   TGenericClassInfo *GenerateInitInstance(const ::eventBuffer::GenJetAK8_s*)
   {
      return GenerateInitInstanceLocal(static_cast<::eventBuffer::GenJetAK8_s*>(nullptr));
   }
   // Static variable to force the class initialization
   static ::ROOT::TGenericClassInfo *_R__UNIQUE_DICT_(Init) = GenerateInitInstanceLocal(static_cast<const ::eventBuffer::GenJetAK8_s*>(nullptr)); R__UseDummy(_R__UNIQUE_DICT_(Init));

   // Dictionary for non-ClassDef classes
   static TClass *eventBuffercLcLGenJetAK8_s_Dictionary() {
      TClass* theClass =::ROOT::GenerateInitInstanceLocal(static_cast<const ::eventBuffer::GenJetAK8_s*>(nullptr))->GetClass();
      eventBuffercLcLGenJetAK8_s_TClassManip(theClass);
   return theClass;
   }

   static void eventBuffercLcLGenJetAK8_s_TClassManip(TClass* ){
   }

} // end of namespace ROOT

namespace ROOT {
   static TClass *eventBuffercLcLGenPart_s_Dictionary();
   static void eventBuffercLcLGenPart_s_TClassManip(TClass*);
   static void *new_eventBuffercLcLGenPart_s(void *p = nullptr);
   static void *newArray_eventBuffercLcLGenPart_s(Long_t size, void *p);
   static void delete_eventBuffercLcLGenPart_s(void *p);
   static void deleteArray_eventBuffercLcLGenPart_s(void *p);
   static void destruct_eventBuffercLcLGenPart_s(void *p);

   // Function generating the singleton type initializer
   static TGenericClassInfo *GenerateInitInstanceLocal(const ::eventBuffer::GenPart_s*)
   {
      ::eventBuffer::GenPart_s *ptr = nullptr;
      static ::TVirtualIsAProxy* isa_proxy = new ::TIsAProxy(typeid(::eventBuffer::GenPart_s));
      static ::ROOT::TGenericClassInfo 
         instance("eventBuffer::GenPart_s", "eventBuffer.h", 1538,
                  typeid(::eventBuffer::GenPart_s), ::ROOT::Internal::DefineBehavior(ptr, ptr),
                  &eventBuffercLcLGenPart_s_Dictionary, isa_proxy, 0,
                  sizeof(::eventBuffer::GenPart_s) );
      instance.SetNew(&new_eventBuffercLcLGenPart_s);
      instance.SetNewArray(&newArray_eventBuffercLcLGenPart_s);
      instance.SetDelete(&delete_eventBuffercLcLGenPart_s);
      instance.SetDeleteArray(&deleteArray_eventBuffercLcLGenPart_s);
      instance.SetDestructor(&destruct_eventBuffercLcLGenPart_s);
      return &instance;
   }
   TGenericClassInfo *GenerateInitInstance(const ::eventBuffer::GenPart_s*)
   {
      return GenerateInitInstanceLocal(static_cast<::eventBuffer::GenPart_s*>(nullptr));
   }
   // Static variable to force the class initialization
   static ::ROOT::TGenericClassInfo *_R__UNIQUE_DICT_(Init) = GenerateInitInstanceLocal(static_cast<const ::eventBuffer::GenPart_s*>(nullptr)); R__UseDummy(_R__UNIQUE_DICT_(Init));

   // Dictionary for non-ClassDef classes
   static TClass *eventBuffercLcLGenPart_s_Dictionary() {
      TClass* theClass =::ROOT::GenerateInitInstanceLocal(static_cast<const ::eventBuffer::GenPart_s*>(nullptr))->GetClass();
      eventBuffercLcLGenPart_s_TClassManip(theClass);
   return theClass;
   }

   static void eventBuffercLcLGenPart_s_TClassManip(TClass* ){
   }

} // end of namespace ROOT

namespace ROOT {
   static TClass *eventBuffercLcLIsoTrack_s_Dictionary();
   static void eventBuffercLcLIsoTrack_s_TClassManip(TClass*);
   static void *new_eventBuffercLcLIsoTrack_s(void *p = nullptr);
   static void *newArray_eventBuffercLcLIsoTrack_s(Long_t size, void *p);
   static void delete_eventBuffercLcLIsoTrack_s(void *p);
   static void deleteArray_eventBuffercLcLIsoTrack_s(void *p);
   static void destruct_eventBuffercLcLIsoTrack_s(void *p);

   // Function generating the singleton type initializer
   static TGenericClassInfo *GenerateInitInstanceLocal(const ::eventBuffer::IsoTrack_s*)
   {
      ::eventBuffer::IsoTrack_s *ptr = nullptr;
      static ::TVirtualIsAProxy* isa_proxy = new ::TIsAProxy(typeid(::eventBuffer::IsoTrack_s));
      static ::ROOT::TGenericClassInfo 
         instance("eventBuffer::IsoTrack_s", "eventBuffer.h", 1565,
                  typeid(::eventBuffer::IsoTrack_s), ::ROOT::Internal::DefineBehavior(ptr, ptr),
                  &eventBuffercLcLIsoTrack_s_Dictionary, isa_proxy, 0,
                  sizeof(::eventBuffer::IsoTrack_s) );
      instance.SetNew(&new_eventBuffercLcLIsoTrack_s);
      instance.SetNewArray(&newArray_eventBuffercLcLIsoTrack_s);
      instance.SetDelete(&delete_eventBuffercLcLIsoTrack_s);
      instance.SetDeleteArray(&deleteArray_eventBuffercLcLIsoTrack_s);
      instance.SetDestructor(&destruct_eventBuffercLcLIsoTrack_s);
      return &instance;
   }
   TGenericClassInfo *GenerateInitInstance(const ::eventBuffer::IsoTrack_s*)
   {
      return GenerateInitInstanceLocal(static_cast<::eventBuffer::IsoTrack_s*>(nullptr));
   }
   // Static variable to force the class initialization
   static ::ROOT::TGenericClassInfo *_R__UNIQUE_DICT_(Init) = GenerateInitInstanceLocal(static_cast<const ::eventBuffer::IsoTrack_s*>(nullptr)); R__UseDummy(_R__UNIQUE_DICT_(Init));

   // Dictionary for non-ClassDef classes
   static TClass *eventBuffercLcLIsoTrack_s_Dictionary() {
      TClass* theClass =::ROOT::GenerateInitInstanceLocal(static_cast<const ::eventBuffer::IsoTrack_s*>(nullptr))->GetClass();
      eventBuffercLcLIsoTrack_s_TClassManip(theClass);
   return theClass;
   }

   static void eventBuffercLcLIsoTrack_s_TClassManip(TClass* ){
   }

} // end of namespace ROOT

namespace ROOT {
   static TClass *eventBuffercLcLJet_s_Dictionary();
   static void eventBuffercLcLJet_s_TClassManip(TClass*);
   static void *new_eventBuffercLcLJet_s(void *p = nullptr);
   static void *newArray_eventBuffercLcLJet_s(Long_t size, void *p);
   static void delete_eventBuffercLcLJet_s(void *p);
   static void deleteArray_eventBuffercLcLJet_s(void *p);
   static void destruct_eventBuffercLcLJet_s(void *p);

   // Function generating the singleton type initializer
   static TGenericClassInfo *GenerateInitInstanceLocal(const ::eventBuffer::Jet_s*)
   {
      ::eventBuffer::Jet_s *ptr = nullptr;
      static ::TVirtualIsAProxy* isa_proxy = new ::TIsAProxy(typeid(::eventBuffer::Jet_s));
      static ::ROOT::TGenericClassInfo 
         instance("eventBuffer::Jet_s", "eventBuffer.h", 1606,
                  typeid(::eventBuffer::Jet_s), ::ROOT::Internal::DefineBehavior(ptr, ptr),
                  &eventBuffercLcLJet_s_Dictionary, isa_proxy, 0,
                  sizeof(::eventBuffer::Jet_s) );
      instance.SetNew(&new_eventBuffercLcLJet_s);
      instance.SetNewArray(&newArray_eventBuffercLcLJet_s);
      instance.SetDelete(&delete_eventBuffercLcLJet_s);
      instance.SetDeleteArray(&deleteArray_eventBuffercLcLJet_s);
      instance.SetDestructor(&destruct_eventBuffercLcLJet_s);
      return &instance;
   }
   TGenericClassInfo *GenerateInitInstance(const ::eventBuffer::Jet_s*)
   {
      return GenerateInitInstanceLocal(static_cast<::eventBuffer::Jet_s*>(nullptr));
   }
   // Static variable to force the class initialization
   static ::ROOT::TGenericClassInfo *_R__UNIQUE_DICT_(Init) = GenerateInitInstanceLocal(static_cast<const ::eventBuffer::Jet_s*>(nullptr)); R__UseDummy(_R__UNIQUE_DICT_(Init));

   // Dictionary for non-ClassDef classes
   static TClass *eventBuffercLcLJet_s_Dictionary() {
      TClass* theClass =::ROOT::GenerateInitInstanceLocal(static_cast<const ::eventBuffer::Jet_s*>(nullptr))->GetClass();
      eventBuffercLcLJet_s_TClassManip(theClass);
   return theClass;
   }

   static void eventBuffercLcLJet_s_TClassManip(TClass* ){
   }

} // end of namespace ROOT

namespace ROOT {
   static TClass *eventBuffercLcLLHEPart_s_Dictionary();
   static void eventBuffercLcLLHEPart_s_TClassManip(TClass*);
   static void *new_eventBuffercLcLLHEPart_s(void *p = nullptr);
   static void *newArray_eventBuffercLcLLHEPart_s(Long_t size, void *p);
   static void delete_eventBuffercLcLLHEPart_s(void *p);
   static void deleteArray_eventBuffercLcLLHEPart_s(void *p);
   static void destruct_eventBuffercLcLLHEPart_s(void *p);

   // Function generating the singleton type initializer
   static TGenericClassInfo *GenerateInitInstanceLocal(const ::eventBuffer::LHEPart_s*)
   {
      ::eventBuffer::LHEPart_s *ptr = nullptr;
      static ::TVirtualIsAProxy* isa_proxy = new ::TIsAProxy(typeid(::eventBuffer::LHEPart_s));
      static ::ROOT::TGenericClassInfo 
         instance("eventBuffer::LHEPart_s", "eventBuffer.h", 1733,
                  typeid(::eventBuffer::LHEPart_s), ::ROOT::Internal::DefineBehavior(ptr, ptr),
                  &eventBuffercLcLLHEPart_s_Dictionary, isa_proxy, 0,
                  sizeof(::eventBuffer::LHEPart_s) );
      instance.SetNew(&new_eventBuffercLcLLHEPart_s);
      instance.SetNewArray(&newArray_eventBuffercLcLLHEPart_s);
      instance.SetDelete(&delete_eventBuffercLcLLHEPart_s);
      instance.SetDeleteArray(&deleteArray_eventBuffercLcLLHEPart_s);
      instance.SetDestructor(&destruct_eventBuffercLcLLHEPart_s);
      return &instance;
   }
   TGenericClassInfo *GenerateInitInstance(const ::eventBuffer::LHEPart_s*)
   {
      return GenerateInitInstanceLocal(static_cast<::eventBuffer::LHEPart_s*>(nullptr));
   }
   // Static variable to force the class initialization
   static ::ROOT::TGenericClassInfo *_R__UNIQUE_DICT_(Init) = GenerateInitInstanceLocal(static_cast<const ::eventBuffer::LHEPart_s*>(nullptr)); R__UseDummy(_R__UNIQUE_DICT_(Init));

   // Dictionary for non-ClassDef classes
   static TClass *eventBuffercLcLLHEPart_s_Dictionary() {
      TClass* theClass =::ROOT::GenerateInitInstanceLocal(static_cast<const ::eventBuffer::LHEPart_s*>(nullptr))->GetClass();
      eventBuffercLcLLHEPart_s_TClassManip(theClass);
   return theClass;
   }

   static void eventBuffercLcLLHEPart_s_TClassManip(TClass* ){
   }

} // end of namespace ROOT

namespace ROOT {
   static TClass *eventBuffercLcLMuon_s_Dictionary();
   static void eventBuffercLcLMuon_s_TClassManip(TClass*);
   static void *new_eventBuffercLcLMuon_s(void *p = nullptr);
   static void *newArray_eventBuffercLcLMuon_s(Long_t size, void *p);
   static void delete_eventBuffercLcLMuon_s(void *p);
   static void deleteArray_eventBuffercLcLMuon_s(void *p);
   static void destruct_eventBuffercLcLMuon_s(void *p);

   // Function generating the singleton type initializer
   static TGenericClassInfo *GenerateInitInstanceLocal(const ::eventBuffer::Muon_s*)
   {
      ::eventBuffer::Muon_s *ptr = nullptr;
      static ::TVirtualIsAProxy* isa_proxy = new ::TIsAProxy(typeid(::eventBuffer::Muon_s));
      static ::ROOT::TGenericClassInfo 
         instance("eventBuffer::Muon_s", "eventBuffer.h", 1760,
                  typeid(::eventBuffer::Muon_s), ::ROOT::Internal::DefineBehavior(ptr, ptr),
                  &eventBuffercLcLMuon_s_Dictionary, isa_proxy, 0,
                  sizeof(::eventBuffer::Muon_s) );
      instance.SetNew(&new_eventBuffercLcLMuon_s);
      instance.SetNewArray(&newArray_eventBuffercLcLMuon_s);
      instance.SetDelete(&delete_eventBuffercLcLMuon_s);
      instance.SetDeleteArray(&deleteArray_eventBuffercLcLMuon_s);
      instance.SetDestructor(&destruct_eventBuffercLcLMuon_s);
      return &instance;
   }
   TGenericClassInfo *GenerateInitInstance(const ::eventBuffer::Muon_s*)
   {
      return GenerateInitInstanceLocal(static_cast<::eventBuffer::Muon_s*>(nullptr));
   }
   // Static variable to force the class initialization
   static ::ROOT::TGenericClassInfo *_R__UNIQUE_DICT_(Init) = GenerateInitInstanceLocal(static_cast<const ::eventBuffer::Muon_s*>(nullptr)); R__UseDummy(_R__UNIQUE_DICT_(Init));

   // Dictionary for non-ClassDef classes
   static TClass *eventBuffercLcLMuon_s_Dictionary() {
      TClass* theClass =::ROOT::GenerateInitInstanceLocal(static_cast<const ::eventBuffer::Muon_s*>(nullptr))->GetClass();
      eventBuffercLcLMuon_s_TClassManip(theClass);
   return theClass;
   }

   static void eventBuffercLcLMuon_s_TClassManip(TClass* ){
   }

} // end of namespace ROOT

namespace ROOT {
   static TClass *eventBuffercLcLPhoton_s_Dictionary();
   static void eventBuffercLcLPhoton_s_TClassManip(TClass*);
   static void *new_eventBuffercLcLPhoton_s(void *p = nullptr);
   static void *newArray_eventBuffercLcLPhoton_s(Long_t size, void *p);
   static void delete_eventBuffercLcLPhoton_s(void *p);
   static void deleteArray_eventBuffercLcLPhoton_s(void *p);
   static void destruct_eventBuffercLcLPhoton_s(void *p);

   // Function generating the singleton type initializer
   static TGenericClassInfo *GenerateInitInstanceLocal(const ::eventBuffer::Photon_s*)
   {
      ::eventBuffer::Photon_s *ptr = nullptr;
      static ::TVirtualIsAProxy* isa_proxy = new ::TIsAProxy(typeid(::eventBuffer::Photon_s));
      static ::ROOT::TGenericClassInfo 
         instance("eventBuffer::Photon_s", "eventBuffer.h", 1885,
                  typeid(::eventBuffer::Photon_s), ::ROOT::Internal::DefineBehavior(ptr, ptr),
                  &eventBuffercLcLPhoton_s_Dictionary, isa_proxy, 0,
                  sizeof(::eventBuffer::Photon_s) );
      instance.SetNew(&new_eventBuffercLcLPhoton_s);
      instance.SetNewArray(&newArray_eventBuffercLcLPhoton_s);
      instance.SetDelete(&delete_eventBuffercLcLPhoton_s);
      instance.SetDeleteArray(&deleteArray_eventBuffercLcLPhoton_s);
      instance.SetDestructor(&destruct_eventBuffercLcLPhoton_s);
      return &instance;
   }
   TGenericClassInfo *GenerateInitInstance(const ::eventBuffer::Photon_s*)
   {
      return GenerateInitInstanceLocal(static_cast<::eventBuffer::Photon_s*>(nullptr));
   }
   // Static variable to force the class initialization
   static ::ROOT::TGenericClassInfo *_R__UNIQUE_DICT_(Init) = GenerateInitInstanceLocal(static_cast<const ::eventBuffer::Photon_s*>(nullptr)); R__UseDummy(_R__UNIQUE_DICT_(Init));

   // Dictionary for non-ClassDef classes
   static TClass *eventBuffercLcLPhoton_s_Dictionary() {
      TClass* theClass =::ROOT::GenerateInitInstanceLocal(static_cast<const ::eventBuffer::Photon_s*>(nullptr))->GetClass();
      eventBuffercLcLPhoton_s_TClassManip(theClass);
   return theClass;
   }

   static void eventBuffercLcLPhoton_s_TClassManip(TClass* ){
   }

} // end of namespace ROOT

namespace ROOT {
   static TClass *eventBuffercLcLSubGenJetAK8_s_Dictionary();
   static void eventBuffercLcLSubGenJetAK8_s_TClassManip(TClass*);
   static void *new_eventBuffercLcLSubGenJetAK8_s(void *p = nullptr);
   static void *newArray_eventBuffercLcLSubGenJetAK8_s(Long_t size, void *p);
   static void delete_eventBuffercLcLSubGenJetAK8_s(void *p);
   static void deleteArray_eventBuffercLcLSubGenJetAK8_s(void *p);
   static void destruct_eventBuffercLcLSubGenJetAK8_s(void *p);

   // Function generating the singleton type initializer
   static TGenericClassInfo *GenerateInitInstanceLocal(const ::eventBuffer::SubGenJetAK8_s*)
   {
      ::eventBuffer::SubGenJetAK8_s *ptr = nullptr;
      static ::TVirtualIsAProxy* isa_proxy = new ::TIsAProxy(typeid(::eventBuffer::SubGenJetAK8_s));
      static ::ROOT::TGenericClassInfo 
         instance("eventBuffer::SubGenJetAK8_s", "eventBuffer.h", 1964,
                  typeid(::eventBuffer::SubGenJetAK8_s), ::ROOT::Internal::DefineBehavior(ptr, ptr),
                  &eventBuffercLcLSubGenJetAK8_s_Dictionary, isa_proxy, 0,
                  sizeof(::eventBuffer::SubGenJetAK8_s) );
      instance.SetNew(&new_eventBuffercLcLSubGenJetAK8_s);
      instance.SetNewArray(&newArray_eventBuffercLcLSubGenJetAK8_s);
      instance.SetDelete(&delete_eventBuffercLcLSubGenJetAK8_s);
      instance.SetDeleteArray(&deleteArray_eventBuffercLcLSubGenJetAK8_s);
      instance.SetDestructor(&destruct_eventBuffercLcLSubGenJetAK8_s);
      return &instance;
   }
   TGenericClassInfo *GenerateInitInstance(const ::eventBuffer::SubGenJetAK8_s*)
   {
      return GenerateInitInstanceLocal(static_cast<::eventBuffer::SubGenJetAK8_s*>(nullptr));
   }
   // Static variable to force the class initialization
   static ::ROOT::TGenericClassInfo *_R__UNIQUE_DICT_(Init) = GenerateInitInstanceLocal(static_cast<const ::eventBuffer::SubGenJetAK8_s*>(nullptr)); R__UseDummy(_R__UNIQUE_DICT_(Init));

   // Dictionary for non-ClassDef classes
   static TClass *eventBuffercLcLSubGenJetAK8_s_Dictionary() {
      TClass* theClass =::ROOT::GenerateInitInstanceLocal(static_cast<const ::eventBuffer::SubGenJetAK8_s*>(nullptr))->GetClass();
      eventBuffercLcLSubGenJetAK8_s_TClassManip(theClass);
   return theClass;
   }

   static void eventBuffercLcLSubGenJetAK8_s_TClassManip(TClass* ){
   }

} // end of namespace ROOT

namespace ROOT {
   static TClass *eventBuffercLcLSubJet_s_Dictionary();
   static void eventBuffercLcLSubJet_s_TClassManip(TClass*);
   static void *new_eventBuffercLcLSubJet_s(void *p = nullptr);
   static void *newArray_eventBuffercLcLSubJet_s(Long_t size, void *p);
   static void delete_eventBuffercLcLSubJet_s(void *p);
   static void deleteArray_eventBuffercLcLSubJet_s(void *p);
   static void destruct_eventBuffercLcLSubJet_s(void *p);

   // Function generating the singleton type initializer
   static TGenericClassInfo *GenerateInitInstanceLocal(const ::eventBuffer::SubJet_s*)
   {
      ::eventBuffer::SubJet_s *ptr = nullptr;
      static ::TVirtualIsAProxy* isa_proxy = new ::TIsAProxy(typeid(::eventBuffer::SubJet_s));
      static ::ROOT::TGenericClassInfo 
         instance("eventBuffer::SubJet_s", "eventBuffer.h", 1983,
                  typeid(::eventBuffer::SubJet_s), ::ROOT::Internal::DefineBehavior(ptr, ptr),
                  &eventBuffercLcLSubJet_s_Dictionary, isa_proxy, 0,
                  sizeof(::eventBuffer::SubJet_s) );
      instance.SetNew(&new_eventBuffercLcLSubJet_s);
      instance.SetNewArray(&newArray_eventBuffercLcLSubJet_s);
      instance.SetDelete(&delete_eventBuffercLcLSubJet_s);
      instance.SetDeleteArray(&deleteArray_eventBuffercLcLSubJet_s);
      instance.SetDestructor(&destruct_eventBuffercLcLSubJet_s);
      return &instance;
   }
   TGenericClassInfo *GenerateInitInstance(const ::eventBuffer::SubJet_s*)
   {
      return GenerateInitInstanceLocal(static_cast<::eventBuffer::SubJet_s*>(nullptr));
   }
   // Static variable to force the class initialization
   static ::ROOT::TGenericClassInfo *_R__UNIQUE_DICT_(Init) = GenerateInitInstanceLocal(static_cast<const ::eventBuffer::SubJet_s*>(nullptr)); R__UseDummy(_R__UNIQUE_DICT_(Init));

   // Dictionary for non-ClassDef classes
   static TClass *eventBuffercLcLSubJet_s_Dictionary() {
      TClass* theClass =::ROOT::GenerateInitInstanceLocal(static_cast<const ::eventBuffer::SubJet_s*>(nullptr))->GetClass();
      eventBuffercLcLSubJet_s_TClassManip(theClass);
   return theClass;
   }

   static void eventBuffercLcLSubJet_s_TClassManip(TClass* ){
   }

} // end of namespace ROOT

namespace ROOT {
   static TClass *eventBuffercLcLTau_s_Dictionary();
   static void eventBuffercLcLTau_s_TClassManip(TClass*);
   static void *new_eventBuffercLcLTau_s(void *p = nullptr);
   static void *newArray_eventBuffercLcLTau_s(Long_t size, void *p);
   static void delete_eventBuffercLcLTau_s(void *p);
   static void deleteArray_eventBuffercLcLTau_s(void *p);
   static void destruct_eventBuffercLcLTau_s(void *p);

   // Function generating the singleton type initializer
   static TGenericClassInfo *GenerateInitInstanceLocal(const ::eventBuffer::Tau_s*)
   {
      ::eventBuffer::Tau_s *ptr = nullptr;
      static ::TVirtualIsAProxy* isa_proxy = new ::TIsAProxy(typeid(::eventBuffer::Tau_s));
      static ::ROOT::TGenericClassInfo 
         instance("eventBuffer::Tau_s", "eventBuffer.h", 2026,
                  typeid(::eventBuffer::Tau_s), ::ROOT::Internal::DefineBehavior(ptr, ptr),
                  &eventBuffercLcLTau_s_Dictionary, isa_proxy, 0,
                  sizeof(::eventBuffer::Tau_s) );
      instance.SetNew(&new_eventBuffercLcLTau_s);
      instance.SetNewArray(&newArray_eventBuffercLcLTau_s);
      instance.SetDelete(&delete_eventBuffercLcLTau_s);
      instance.SetDeleteArray(&deleteArray_eventBuffercLcLTau_s);
      instance.SetDestructor(&destruct_eventBuffercLcLTau_s);
      return &instance;
   }
   TGenericClassInfo *GenerateInitInstance(const ::eventBuffer::Tau_s*)
   {
      return GenerateInitInstanceLocal(static_cast<::eventBuffer::Tau_s*>(nullptr));
   }
   // Static variable to force the class initialization
   static ::ROOT::TGenericClassInfo *_R__UNIQUE_DICT_(Init) = GenerateInitInstanceLocal(static_cast<const ::eventBuffer::Tau_s*>(nullptr)); R__UseDummy(_R__UNIQUE_DICT_(Init));

   // Dictionary for non-ClassDef classes
   static TClass *eventBuffercLcLTau_s_Dictionary() {
      TClass* theClass =::ROOT::GenerateInitInstanceLocal(static_cast<const ::eventBuffer::Tau_s*>(nullptr))->GetClass();
      eventBuffercLcLTau_s_TClassManip(theClass);
   return theClass;
   }

   static void eventBuffercLcLTau_s_TClassManip(TClass* ){
   }

} // end of namespace ROOT

namespace ROOT {
   static TClass *eventBuffercLcLTrigObj_s_Dictionary();
   static void eventBuffercLcLTrigObj_s_TClassManip(TClass*);
   static void *new_eventBuffercLcLTrigObj_s(void *p = nullptr);
   static void *newArray_eventBuffercLcLTrigObj_s(Long_t size, void *p);
   static void delete_eventBuffercLcLTrigObj_s(void *p);
   static void deleteArray_eventBuffercLcLTrigObj_s(void *p);
   static void destruct_eventBuffercLcLTrigObj_s(void *p);

   // Function generating the singleton type initializer
   static TGenericClassInfo *GenerateInitInstanceLocal(const ::eventBuffer::TrigObj_s*)
   {
      ::eventBuffer::TrigObj_s *ptr = nullptr;
      static ::TVirtualIsAProxy* isa_proxy = new ::TIsAProxy(typeid(::eventBuffer::TrigObj_s));
      static ::ROOT::TGenericClassInfo 
         instance("eventBuffer::TrigObj_s", "eventBuffer.h", 2097,
                  typeid(::eventBuffer::TrigObj_s), ::ROOT::Internal::DefineBehavior(ptr, ptr),
                  &eventBuffercLcLTrigObj_s_Dictionary, isa_proxy, 0,
                  sizeof(::eventBuffer::TrigObj_s) );
      instance.SetNew(&new_eventBuffercLcLTrigObj_s);
      instance.SetNewArray(&newArray_eventBuffercLcLTrigObj_s);
      instance.SetDelete(&delete_eventBuffercLcLTrigObj_s);
      instance.SetDeleteArray(&deleteArray_eventBuffercLcLTrigObj_s);
      instance.SetDestructor(&destruct_eventBuffercLcLTrigObj_s);
      return &instance;
   }
   TGenericClassInfo *GenerateInitInstance(const ::eventBuffer::TrigObj_s*)
   {
      return GenerateInitInstanceLocal(static_cast<::eventBuffer::TrigObj_s*>(nullptr));
   }
   // Static variable to force the class initialization
   static ::ROOT::TGenericClassInfo *_R__UNIQUE_DICT_(Init) = GenerateInitInstanceLocal(static_cast<const ::eventBuffer::TrigObj_s*>(nullptr)); R__UseDummy(_R__UNIQUE_DICT_(Init));

   // Dictionary for non-ClassDef classes
   static TClass *eventBuffercLcLTrigObj_s_Dictionary() {
      TClass* theClass =::ROOT::GenerateInitInstanceLocal(static_cast<const ::eventBuffer::TrigObj_s*>(nullptr))->GetClass();
      eventBuffercLcLTrigObj_s_TClassManip(theClass);
   return theClass;
   }

   static void eventBuffercLcLTrigObj_s_TClassManip(TClass* ){
   }

} // end of namespace ROOT

namespace ROOT {
   static TClass *eventBuffercLcLboostedTau_s_Dictionary();
   static void eventBuffercLcLboostedTau_s_TClassManip(TClass*);
   static void *new_eventBuffercLcLboostedTau_s(void *p = nullptr);
   static void *newArray_eventBuffercLcLboostedTau_s(Long_t size, void *p);
   static void delete_eventBuffercLcLboostedTau_s(void *p);
   static void deleteArray_eventBuffercLcLboostedTau_s(void *p);
   static void destruct_eventBuffercLcLboostedTau_s(void *p);

   // Function generating the singleton type initializer
   static TGenericClassInfo *GenerateInitInstanceLocal(const ::eventBuffer::boostedTau_s*)
   {
      ::eventBuffer::boostedTau_s *ptr = nullptr;
      static ::TVirtualIsAProxy* isa_proxy = new ::TIsAProxy(typeid(::eventBuffer::boostedTau_s));
      static ::ROOT::TGenericClassInfo 
         instance("eventBuffer::boostedTau_s", "eventBuffer.h", 2128,
                  typeid(::eventBuffer::boostedTau_s), ::ROOT::Internal::DefineBehavior(ptr, ptr),
                  &eventBuffercLcLboostedTau_s_Dictionary, isa_proxy, 0,
                  sizeof(::eventBuffer::boostedTau_s) );
      instance.SetNew(&new_eventBuffercLcLboostedTau_s);
      instance.SetNewArray(&newArray_eventBuffercLcLboostedTau_s);
      instance.SetDelete(&delete_eventBuffercLcLboostedTau_s);
      instance.SetDeleteArray(&deleteArray_eventBuffercLcLboostedTau_s);
      instance.SetDestructor(&destruct_eventBuffercLcLboostedTau_s);
      return &instance;
   }
   TGenericClassInfo *GenerateInitInstance(const ::eventBuffer::boostedTau_s*)
   {
      return GenerateInitInstanceLocal(static_cast<::eventBuffer::boostedTau_s*>(nullptr));
   }
   // Static variable to force the class initialization
   static ::ROOT::TGenericClassInfo *_R__UNIQUE_DICT_(Init) = GenerateInitInstanceLocal(static_cast<const ::eventBuffer::boostedTau_s*>(nullptr)); R__UseDummy(_R__UNIQUE_DICT_(Init));

   // Dictionary for non-ClassDef classes
   static TClass *eventBuffercLcLboostedTau_s_Dictionary() {
      TClass* theClass =::ROOT::GenerateInitInstanceLocal(static_cast<const ::eventBuffer::boostedTau_s*>(nullptr))->GetClass();
      eventBuffercLcLboostedTau_s_TClassManip(theClass);
   return theClass;
   }

   static void eventBuffercLcLboostedTau_s_TClassManip(TClass* ){
   }

} // end of namespace ROOT

namespace ROOT {
   static TClass *outputFile_Dictionary();
   static void outputFile_TClassManip(TClass*);
   static void delete_outputFile(void *p);
   static void deleteArray_outputFile(void *p);
   static void destruct_outputFile(void *p);

   // Function generating the singleton type initializer
   static TGenericClassInfo *GenerateInitInstanceLocal(const ::outputFile*)
   {
      ::outputFile *ptr = nullptr;
      static ::TVirtualIsAProxy* isa_proxy = new ::TIsAProxy(typeid(::outputFile));
      static ::ROOT::TGenericClassInfo 
         instance("outputFile", "tnm.h", 31,
                  typeid(::outputFile), ::ROOT::Internal::DefineBehavior(ptr, ptr),
                  &outputFile_Dictionary, isa_proxy, 0,
                  sizeof(::outputFile) );
      instance.SetDelete(&delete_outputFile);
      instance.SetDeleteArray(&deleteArray_outputFile);
      instance.SetDestructor(&destruct_outputFile);
      return &instance;
   }
   TGenericClassInfo *GenerateInitInstance(const ::outputFile*)
   {
      return GenerateInitInstanceLocal(static_cast<::outputFile*>(nullptr));
   }
   // Static variable to force the class initialization
   static ::ROOT::TGenericClassInfo *_R__UNIQUE_DICT_(Init) = GenerateInitInstanceLocal(static_cast<const ::outputFile*>(nullptr)); R__UseDummy(_R__UNIQUE_DICT_(Init));

   // Dictionary for non-ClassDef classes
   static TClass *outputFile_Dictionary() {
      TClass* theClass =::ROOT::GenerateInitInstanceLocal(static_cast<const ::outputFile*>(nullptr))->GetClass();
      outputFile_TClassManip(theClass);
   return theClass;
   }

   static void outputFile_TClassManip(TClass* ){
   }

} // end of namespace ROOT

namespace ROOT {
   static TClass *commandLine_Dictionary();
   static void commandLine_TClassManip(TClass*);
   static void *new_commandLine(void *p = nullptr);
   static void *newArray_commandLine(Long_t size, void *p);
   static void delete_commandLine(void *p);
   static void deleteArray_commandLine(void *p);
   static void destruct_commandLine(void *p);

   // Function generating the singleton type initializer
   static TGenericClassInfo *GenerateInitInstanceLocal(const ::commandLine*)
   {
      ::commandLine *ptr = nullptr;
      static ::TVirtualIsAProxy* isa_proxy = new ::TIsAProxy(typeid(::commandLine));
      static ::ROOT::TGenericClassInfo 
         instance("commandLine", "tnm.h", 50,
                  typeid(::commandLine), ::ROOT::Internal::DefineBehavior(ptr, ptr),
                  &commandLine_Dictionary, isa_proxy, 0,
                  sizeof(::commandLine) );
      instance.SetNew(&new_commandLine);
      instance.SetNewArray(&newArray_commandLine);
      instance.SetDelete(&delete_commandLine);
      instance.SetDeleteArray(&deleteArray_commandLine);
      instance.SetDestructor(&destruct_commandLine);
      return &instance;
   }
   TGenericClassInfo *GenerateInitInstance(const ::commandLine*)
   {
      return GenerateInitInstanceLocal(static_cast<::commandLine*>(nullptr));
   }
   // Static variable to force the class initialization
   static ::ROOT::TGenericClassInfo *_R__UNIQUE_DICT_(Init) = GenerateInitInstanceLocal(static_cast<const ::commandLine*>(nullptr)); R__UseDummy(_R__UNIQUE_DICT_(Init));

   // Dictionary for non-ClassDef classes
   static TClass *commandLine_Dictionary() {
      TClass* theClass =::ROOT::GenerateInitInstanceLocal(static_cast<const ::commandLine*>(nullptr))->GetClass();
      commandLine_TClassManip(theClass);
   return theClass;
   }

   static void commandLine_TClassManip(TClass* ){
   }

} // end of namespace ROOT

namespace ROOT {
   static TClass *matchedPair_Dictionary();
   static void matchedPair_TClassManip(TClass*);
   static void *new_matchedPair(void *p = nullptr);
   static void *newArray_matchedPair(Long_t size, void *p);
   static void delete_matchedPair(void *p);
   static void deleteArray_matchedPair(void *p);
   static void destruct_matchedPair(void *p);

   // Function generating the singleton type initializer
   static TGenericClassInfo *GenerateInitInstanceLocal(const ::matchedPair*)
   {
      ::matchedPair *ptr = nullptr;
      static ::TVirtualIsAProxy* isa_proxy = new ::TIsAProxy(typeid(::matchedPair));
      static ::ROOT::TGenericClassInfo 
         instance("matchedPair", "tnm.h", 69,
                  typeid(::matchedPair), ::ROOT::Internal::DefineBehavior(ptr, ptr),
                  &matchedPair_Dictionary, isa_proxy, 0,
                  sizeof(::matchedPair) );
      instance.SetNew(&new_matchedPair);
      instance.SetNewArray(&newArray_matchedPair);
      instance.SetDelete(&delete_matchedPair);
      instance.SetDeleteArray(&deleteArray_matchedPair);
      instance.SetDestructor(&destruct_matchedPair);
      return &instance;
   }
   TGenericClassInfo *GenerateInitInstance(const ::matchedPair*)
   {
      return GenerateInitInstanceLocal(static_cast<::matchedPair*>(nullptr));
   }
   // Static variable to force the class initialization
   static ::ROOT::TGenericClassInfo *_R__UNIQUE_DICT_(Init) = GenerateInitInstanceLocal(static_cast<const ::matchedPair*>(nullptr)); R__UseDummy(_R__UNIQUE_DICT_(Init));

   // Dictionary for non-ClassDef classes
   static TClass *matchedPair_Dictionary() {
      TClass* theClass =::ROOT::GenerateInitInstanceLocal(static_cast<const ::matchedPair*>(nullptr))->GetClass();
      matchedPair_TClassManip(theClass);
   return theClass;
   }

   static void matchedPair_TClassManip(TClass* ){
   }

} // end of namespace ROOT

namespace ROOT {
   static TClass *ptThing_Dictionary();
   static void ptThing_TClassManip(TClass*);
   static void *new_ptThing(void *p = nullptr);
   static void *newArray_ptThing(Long_t size, void *p);
   static void delete_ptThing(void *p);
   static void deleteArray_ptThing(void *p);
   static void destruct_ptThing(void *p);

   // Function generating the singleton type initializer
   static TGenericClassInfo *GenerateInitInstanceLocal(const ::ptThing*)
   {
      ::ptThing *ptr = nullptr;
      static ::TVirtualIsAProxy* isa_proxy = new ::TIsAProxy(typeid(::ptThing));
      static ::ROOT::TGenericClassInfo 
         instance("ptThing", "tnm.h", 79,
                  typeid(::ptThing), ::ROOT::Internal::DefineBehavior(ptr, ptr),
                  &ptThing_Dictionary, isa_proxy, 0,
                  sizeof(::ptThing) );
      instance.SetNew(&new_ptThing);
      instance.SetNewArray(&newArray_ptThing);
      instance.SetDelete(&delete_ptThing);
      instance.SetDeleteArray(&deleteArray_ptThing);
      instance.SetDestructor(&destruct_ptThing);
      return &instance;
   }
   TGenericClassInfo *GenerateInitInstance(const ::ptThing*)
   {
      return GenerateInitInstanceLocal(static_cast<::ptThing*>(nullptr));
   }
   // Static variable to force the class initialization
   static ::ROOT::TGenericClassInfo *_R__UNIQUE_DICT_(Init) = GenerateInitInstanceLocal(static_cast<const ::ptThing*>(nullptr)); R__UseDummy(_R__UNIQUE_DICT_(Init));

   // Dictionary for non-ClassDef classes
   static TClass *ptThing_Dictionary() {
      TClass* theClass =::ROOT::GenerateInitInstanceLocal(static_cast<const ::ptThing*>(nullptr))->GetClass();
      ptThing_TClassManip(theClass);
   return theClass;
   }

   static void ptThing_TClassManip(TClass* ){
   }

} // end of namespace ROOT

//______________________________________________________________________________
atomic_TClass_ptr itreestream::fgIsA(nullptr);  // static to hold class pointer

//______________________________________________________________________________
const char *itreestream::Class_Name()
{
   return "itreestream";
}

//______________________________________________________________________________
const char *itreestream::ImplFileName()
{
   return ::ROOT::GenerateInitInstanceLocal((const ::itreestream*)nullptr)->GetImplFileName();
}

//______________________________________________________________________________
int itreestream::ImplFileLine()
{
   return ::ROOT::GenerateInitInstanceLocal((const ::itreestream*)nullptr)->GetImplFileLine();
}

//______________________________________________________________________________
TClass *itreestream::Dictionary()
{
   fgIsA = ::ROOT::GenerateInitInstanceLocal((const ::itreestream*)nullptr)->GetClass();
   return fgIsA;
}

//______________________________________________________________________________
TClass *itreestream::Class()
{
   if (!fgIsA.load()) { R__LOCKGUARD(gInterpreterMutex); fgIsA = ::ROOT::GenerateInitInstanceLocal((const ::itreestream*)nullptr)->GetClass(); }
   return fgIsA;
}

//______________________________________________________________________________
atomic_TClass_ptr otreestream::fgIsA(nullptr);  // static to hold class pointer

//______________________________________________________________________________
const char *otreestream::Class_Name()
{
   return "otreestream";
}

//______________________________________________________________________________
const char *otreestream::ImplFileName()
{
   return ::ROOT::GenerateInitInstanceLocal((const ::otreestream*)nullptr)->GetImplFileName();
}

//______________________________________________________________________________
int otreestream::ImplFileLine()
{
   return ::ROOT::GenerateInitInstanceLocal((const ::otreestream*)nullptr)->GetImplFileLine();
}

//______________________________________________________________________________
TClass *otreestream::Dictionary()
{
   fgIsA = ::ROOT::GenerateInitInstanceLocal((const ::otreestream*)nullptr)->GetClass();
   return fgIsA;
}

//______________________________________________________________________________
TClass *otreestream::Class()
{
   if (!fgIsA.load()) { R__LOCKGUARD(gInterpreterMutex); fgIsA = ::ROOT::GenerateInitInstanceLocal((const ::otreestream*)nullptr)->GetClass(); }
   return fgIsA;
}

//______________________________________________________________________________
atomic_TClass_ptr eventBuffer::fgIsA(nullptr);  // static to hold class pointer

//______________________________________________________________________________
const char *eventBuffer::Class_Name()
{
   return "eventBuffer";
}

//______________________________________________________________________________
const char *eventBuffer::ImplFileName()
{
   return ::ROOT::GenerateInitInstanceLocal((const ::eventBuffer*)nullptr)->GetImplFileName();
}

//______________________________________________________________________________
int eventBuffer::ImplFileLine()
{
   return ::ROOT::GenerateInitInstanceLocal((const ::eventBuffer*)nullptr)->GetImplFileLine();
}

//______________________________________________________________________________
TClass *eventBuffer::Dictionary()
{
   fgIsA = ::ROOT::GenerateInitInstanceLocal((const ::eventBuffer*)nullptr)->GetClass();
   return fgIsA;
}

//______________________________________________________________________________
TClass *eventBuffer::Class()
{
   if (!fgIsA.load()) { R__LOCKGUARD(gInterpreterMutex); fgIsA = ::ROOT::GenerateInitInstanceLocal((const ::eventBuffer*)nullptr)->GetClass(); }
   return fgIsA;
}

//______________________________________________________________________________
void itreestream::Streamer(TBuffer &R__b)
{
   // Stream an object of class itreestream.

   if (R__b.IsReading()) {
      R__b.ReadClassBuffer(itreestream::Class(),this);
   } else {
      R__b.WriteClassBuffer(itreestream::Class(),this);
   }
}

namespace ROOT {
   // Wrappers around operator new
   static void *new_itreestream(void *p) {
      return  p ? new(p) ::itreestream : new ::itreestream;
   }
   static void *newArray_itreestream(Long_t nElements, void *p) {
      return p ? new(p) ::itreestream[nElements] : new ::itreestream[nElements];
   }
   // Wrapper around operator delete
   static void delete_itreestream(void *p) {
      delete (static_cast<::itreestream*>(p));
   }
   static void deleteArray_itreestream(void *p) {
      delete [] (static_cast<::itreestream*>(p));
   }
   static void destruct_itreestream(void *p) {
      typedef ::itreestream current_t;
      (static_cast<current_t*>(p))->~current_t();
   }
} // end of namespace ROOT for class ::itreestream

//______________________________________________________________________________
void otreestream::Streamer(TBuffer &R__b)
{
   // Stream an object of class otreestream.

   if (R__b.IsReading()) {
      R__b.ReadClassBuffer(otreestream::Class(),this);
   } else {
      R__b.WriteClassBuffer(otreestream::Class(),this);
   }
}

namespace ROOT {
   // Wrappers around operator new
   static void *new_otreestream(void *p) {
      return  p ? new(p) ::otreestream : new ::otreestream;
   }
   static void *newArray_otreestream(Long_t nElements, void *p) {
      return p ? new(p) ::otreestream[nElements] : new ::otreestream[nElements];
   }
   // Wrapper around operator delete
   static void delete_otreestream(void *p) {
      delete (static_cast<::otreestream*>(p));
   }
   static void deleteArray_otreestream(void *p) {
      delete [] (static_cast<::otreestream*>(p));
   }
   static void destruct_otreestream(void *p) {
      typedef ::otreestream current_t;
      (static_cast<current_t*>(p))->~current_t();
   }
} // end of namespace ROOT for class ::otreestream

//______________________________________________________________________________
void eventBuffer::Streamer(TBuffer &R__b)
{
   // Stream an object of class eventBuffer.

   if (R__b.IsReading()) {
      R__b.ReadClassBuffer(eventBuffer::Class(),this);
   } else {
      R__b.WriteClassBuffer(eventBuffer::Class(),this);
   }
}

namespace ROOT {
   // Wrappers around operator new
   static void *new_eventBuffer(void *p) {
      return  p ? new(p) ::eventBuffer : new ::eventBuffer;
   }
   static void *newArray_eventBuffer(Long_t nElements, void *p) {
      return p ? new(p) ::eventBuffer[nElements] : new ::eventBuffer[nElements];
   }
   // Wrapper around operator delete
   static void delete_eventBuffer(void *p) {
      delete (static_cast<::eventBuffer*>(p));
   }
   static void deleteArray_eventBuffer(void *p) {
      delete [] (static_cast<::eventBuffer*>(p));
   }
   static void destruct_eventBuffer(void *p) {
      typedef ::eventBuffer current_t;
      (static_cast<current_t*>(p))->~current_t();
   }
} // end of namespace ROOT for class ::eventBuffer

namespace ROOT {
   // Wrappers around operator new
   static void *new_eventBuffercLcLCorrT1METJet_s(void *p) {
      return  p ? ::new(static_cast<::ROOT::Internal::TOperatorNewHelper*>(p)) ::eventBuffer::CorrT1METJet_s : new ::eventBuffer::CorrT1METJet_s;
   }
   static void *newArray_eventBuffercLcLCorrT1METJet_s(Long_t nElements, void *p) {
      return p ? ::new(static_cast<::ROOT::Internal::TOperatorNewHelper*>(p)) ::eventBuffer::CorrT1METJet_s[nElements] : new ::eventBuffer::CorrT1METJet_s[nElements];
   }
   // Wrapper around operator delete
   static void delete_eventBuffercLcLCorrT1METJet_s(void *p) {
      delete (static_cast<::eventBuffer::CorrT1METJet_s*>(p));
   }
   static void deleteArray_eventBuffercLcLCorrT1METJet_s(void *p) {
      delete [] (static_cast<::eventBuffer::CorrT1METJet_s*>(p));
   }
   static void destruct_eventBuffercLcLCorrT1METJet_s(void *p) {
      typedef ::eventBuffer::CorrT1METJet_s current_t;
      (static_cast<current_t*>(p))->~current_t();
   }
} // end of namespace ROOT for class ::eventBuffer::CorrT1METJet_s

namespace ROOT {
   // Wrappers around operator new
   static void *new_eventBuffercLcLElectron_s(void *p) {
      return  p ? ::new(static_cast<::ROOT::Internal::TOperatorNewHelper*>(p)) ::eventBuffer::Electron_s : new ::eventBuffer::Electron_s;
   }
   static void *newArray_eventBuffercLcLElectron_s(Long_t nElements, void *p) {
      return p ? ::new(static_cast<::ROOT::Internal::TOperatorNewHelper*>(p)) ::eventBuffer::Electron_s[nElements] : new ::eventBuffer::Electron_s[nElements];
   }
   // Wrapper around operator delete
   static void delete_eventBuffercLcLElectron_s(void *p) {
      delete (static_cast<::eventBuffer::Electron_s*>(p));
   }
   static void deleteArray_eventBuffercLcLElectron_s(void *p) {
      delete [] (static_cast<::eventBuffer::Electron_s*>(p));
   }
   static void destruct_eventBuffercLcLElectron_s(void *p) {
      typedef ::eventBuffer::Electron_s current_t;
      (static_cast<current_t*>(p))->~current_t();
   }
} // end of namespace ROOT for class ::eventBuffer::Electron_s

namespace ROOT {
   // Wrappers around operator new
   static void *new_eventBuffercLcLFatJet_s(void *p) {
      return  p ? ::new(static_cast<::ROOT::Internal::TOperatorNewHelper*>(p)) ::eventBuffer::FatJet_s : new ::eventBuffer::FatJet_s;
   }
   static void *newArray_eventBuffercLcLFatJet_s(Long_t nElements, void *p) {
      return p ? ::new(static_cast<::ROOT::Internal::TOperatorNewHelper*>(p)) ::eventBuffer::FatJet_s[nElements] : new ::eventBuffer::FatJet_s[nElements];
   }
   // Wrapper around operator delete
   static void delete_eventBuffercLcLFatJet_s(void *p) {
      delete (static_cast<::eventBuffer::FatJet_s*>(p));
   }
   static void deleteArray_eventBuffercLcLFatJet_s(void *p) {
      delete [] (static_cast<::eventBuffer::FatJet_s*>(p));
   }
   static void destruct_eventBuffercLcLFatJet_s(void *p) {
      typedef ::eventBuffer::FatJet_s current_t;
      (static_cast<current_t*>(p))->~current_t();
   }
} // end of namespace ROOT for class ::eventBuffer::FatJet_s

namespace ROOT {
   // Wrappers around operator new
   static void *new_eventBuffercLcLFsrPhoton_s(void *p) {
      return  p ? ::new(static_cast<::ROOT::Internal::TOperatorNewHelper*>(p)) ::eventBuffer::FsrPhoton_s : new ::eventBuffer::FsrPhoton_s;
   }
   static void *newArray_eventBuffercLcLFsrPhoton_s(Long_t nElements, void *p) {
      return p ? ::new(static_cast<::ROOT::Internal::TOperatorNewHelper*>(p)) ::eventBuffer::FsrPhoton_s[nElements] : new ::eventBuffer::FsrPhoton_s[nElements];
   }
   // Wrapper around operator delete
   static void delete_eventBuffercLcLFsrPhoton_s(void *p) {
      delete (static_cast<::eventBuffer::FsrPhoton_s*>(p));
   }
   static void deleteArray_eventBuffercLcLFsrPhoton_s(void *p) {
      delete [] (static_cast<::eventBuffer::FsrPhoton_s*>(p));
   }
   static void destruct_eventBuffercLcLFsrPhoton_s(void *p) {
      typedef ::eventBuffer::FsrPhoton_s current_t;
      (static_cast<current_t*>(p))->~current_t();
   }
} // end of namespace ROOT for class ::eventBuffer::FsrPhoton_s

namespace ROOT {
   // Wrappers around operator new
   static void *new_eventBuffercLcLGenIsolatedPhoton_s(void *p) {
      return  p ? ::new(static_cast<::ROOT::Internal::TOperatorNewHelper*>(p)) ::eventBuffer::GenIsolatedPhoton_s : new ::eventBuffer::GenIsolatedPhoton_s;
   }
   static void *newArray_eventBuffercLcLGenIsolatedPhoton_s(Long_t nElements, void *p) {
      return p ? ::new(static_cast<::ROOT::Internal::TOperatorNewHelper*>(p)) ::eventBuffer::GenIsolatedPhoton_s[nElements] : new ::eventBuffer::GenIsolatedPhoton_s[nElements];
   }
   // Wrapper around operator delete
   static void delete_eventBuffercLcLGenIsolatedPhoton_s(void *p) {
      delete (static_cast<::eventBuffer::GenIsolatedPhoton_s*>(p));
   }
   static void deleteArray_eventBuffercLcLGenIsolatedPhoton_s(void *p) {
      delete [] (static_cast<::eventBuffer::GenIsolatedPhoton_s*>(p));
   }
   static void destruct_eventBuffercLcLGenIsolatedPhoton_s(void *p) {
      typedef ::eventBuffer::GenIsolatedPhoton_s current_t;
      (static_cast<current_t*>(p))->~current_t();
   }
} // end of namespace ROOT for class ::eventBuffer::GenIsolatedPhoton_s

namespace ROOT {
   // Wrappers around operator new
   static void *new_eventBuffercLcLGenJet_s(void *p) {
      return  p ? ::new(static_cast<::ROOT::Internal::TOperatorNewHelper*>(p)) ::eventBuffer::GenJet_s : new ::eventBuffer::GenJet_s;
   }
   static void *newArray_eventBuffercLcLGenJet_s(Long_t nElements, void *p) {
      return p ? ::new(static_cast<::ROOT::Internal::TOperatorNewHelper*>(p)) ::eventBuffer::GenJet_s[nElements] : new ::eventBuffer::GenJet_s[nElements];
   }
   // Wrapper around operator delete
   static void delete_eventBuffercLcLGenJet_s(void *p) {
      delete (static_cast<::eventBuffer::GenJet_s*>(p));
   }
   static void deleteArray_eventBuffercLcLGenJet_s(void *p) {
      delete [] (static_cast<::eventBuffer::GenJet_s*>(p));
   }
   static void destruct_eventBuffercLcLGenJet_s(void *p) {
      typedef ::eventBuffer::GenJet_s current_t;
      (static_cast<current_t*>(p))->~current_t();
   }
} // end of namespace ROOT for class ::eventBuffer::GenJet_s

namespace ROOT {
   // Wrappers around operator new
   static void *new_eventBuffercLcLGenJetAK8_s(void *p) {
      return  p ? ::new(static_cast<::ROOT::Internal::TOperatorNewHelper*>(p)) ::eventBuffer::GenJetAK8_s : new ::eventBuffer::GenJetAK8_s;
   }
   static void *newArray_eventBuffercLcLGenJetAK8_s(Long_t nElements, void *p) {
      return p ? ::new(static_cast<::ROOT::Internal::TOperatorNewHelper*>(p)) ::eventBuffer::GenJetAK8_s[nElements] : new ::eventBuffer::GenJetAK8_s[nElements];
   }
   // Wrapper around operator delete
   static void delete_eventBuffercLcLGenJetAK8_s(void *p) {
      delete (static_cast<::eventBuffer::GenJetAK8_s*>(p));
   }
   static void deleteArray_eventBuffercLcLGenJetAK8_s(void *p) {
      delete [] (static_cast<::eventBuffer::GenJetAK8_s*>(p));
   }
   static void destruct_eventBuffercLcLGenJetAK8_s(void *p) {
      typedef ::eventBuffer::GenJetAK8_s current_t;
      (static_cast<current_t*>(p))->~current_t();
   }
} // end of namespace ROOT for class ::eventBuffer::GenJetAK8_s

namespace ROOT {
   // Wrappers around operator new
   static void *new_eventBuffercLcLGenPart_s(void *p) {
      return  p ? ::new(static_cast<::ROOT::Internal::TOperatorNewHelper*>(p)) ::eventBuffer::GenPart_s : new ::eventBuffer::GenPart_s;
   }
   static void *newArray_eventBuffercLcLGenPart_s(Long_t nElements, void *p) {
      return p ? ::new(static_cast<::ROOT::Internal::TOperatorNewHelper*>(p)) ::eventBuffer::GenPart_s[nElements] : new ::eventBuffer::GenPart_s[nElements];
   }
   // Wrapper around operator delete
   static void delete_eventBuffercLcLGenPart_s(void *p) {
      delete (static_cast<::eventBuffer::GenPart_s*>(p));
   }
   static void deleteArray_eventBuffercLcLGenPart_s(void *p) {
      delete [] (static_cast<::eventBuffer::GenPart_s*>(p));
   }
   static void destruct_eventBuffercLcLGenPart_s(void *p) {
      typedef ::eventBuffer::GenPart_s current_t;
      (static_cast<current_t*>(p))->~current_t();
   }
} // end of namespace ROOT for class ::eventBuffer::GenPart_s

namespace ROOT {
   // Wrappers around operator new
   static void *new_eventBuffercLcLIsoTrack_s(void *p) {
      return  p ? ::new(static_cast<::ROOT::Internal::TOperatorNewHelper*>(p)) ::eventBuffer::IsoTrack_s : new ::eventBuffer::IsoTrack_s;
   }
   static void *newArray_eventBuffercLcLIsoTrack_s(Long_t nElements, void *p) {
      return p ? ::new(static_cast<::ROOT::Internal::TOperatorNewHelper*>(p)) ::eventBuffer::IsoTrack_s[nElements] : new ::eventBuffer::IsoTrack_s[nElements];
   }
   // Wrapper around operator delete
   static void delete_eventBuffercLcLIsoTrack_s(void *p) {
      delete (static_cast<::eventBuffer::IsoTrack_s*>(p));
   }
   static void deleteArray_eventBuffercLcLIsoTrack_s(void *p) {
      delete [] (static_cast<::eventBuffer::IsoTrack_s*>(p));
   }
   static void destruct_eventBuffercLcLIsoTrack_s(void *p) {
      typedef ::eventBuffer::IsoTrack_s current_t;
      (static_cast<current_t*>(p))->~current_t();
   }
} // end of namespace ROOT for class ::eventBuffer::IsoTrack_s

namespace ROOT {
   // Wrappers around operator new
   static void *new_eventBuffercLcLJet_s(void *p) {
      return  p ? ::new(static_cast<::ROOT::Internal::TOperatorNewHelper*>(p)) ::eventBuffer::Jet_s : new ::eventBuffer::Jet_s;
   }
   static void *newArray_eventBuffercLcLJet_s(Long_t nElements, void *p) {
      return p ? ::new(static_cast<::ROOT::Internal::TOperatorNewHelper*>(p)) ::eventBuffer::Jet_s[nElements] : new ::eventBuffer::Jet_s[nElements];
   }
   // Wrapper around operator delete
   static void delete_eventBuffercLcLJet_s(void *p) {
      delete (static_cast<::eventBuffer::Jet_s*>(p));
   }
   static void deleteArray_eventBuffercLcLJet_s(void *p) {
      delete [] (static_cast<::eventBuffer::Jet_s*>(p));
   }
   static void destruct_eventBuffercLcLJet_s(void *p) {
      typedef ::eventBuffer::Jet_s current_t;
      (static_cast<current_t*>(p))->~current_t();
   }
} // end of namespace ROOT for class ::eventBuffer::Jet_s

namespace ROOT {
   // Wrappers around operator new
   static void *new_eventBuffercLcLLHEPart_s(void *p) {
      return  p ? ::new(static_cast<::ROOT::Internal::TOperatorNewHelper*>(p)) ::eventBuffer::LHEPart_s : new ::eventBuffer::LHEPart_s;
   }
   static void *newArray_eventBuffercLcLLHEPart_s(Long_t nElements, void *p) {
      return p ? ::new(static_cast<::ROOT::Internal::TOperatorNewHelper*>(p)) ::eventBuffer::LHEPart_s[nElements] : new ::eventBuffer::LHEPart_s[nElements];
   }
   // Wrapper around operator delete
   static void delete_eventBuffercLcLLHEPart_s(void *p) {
      delete (static_cast<::eventBuffer::LHEPart_s*>(p));
   }
   static void deleteArray_eventBuffercLcLLHEPart_s(void *p) {
      delete [] (static_cast<::eventBuffer::LHEPart_s*>(p));
   }
   static void destruct_eventBuffercLcLLHEPart_s(void *p) {
      typedef ::eventBuffer::LHEPart_s current_t;
      (static_cast<current_t*>(p))->~current_t();
   }
} // end of namespace ROOT for class ::eventBuffer::LHEPart_s

namespace ROOT {
   // Wrappers around operator new
   static void *new_eventBuffercLcLMuon_s(void *p) {
      return  p ? ::new(static_cast<::ROOT::Internal::TOperatorNewHelper*>(p)) ::eventBuffer::Muon_s : new ::eventBuffer::Muon_s;
   }
   static void *newArray_eventBuffercLcLMuon_s(Long_t nElements, void *p) {
      return p ? ::new(static_cast<::ROOT::Internal::TOperatorNewHelper*>(p)) ::eventBuffer::Muon_s[nElements] : new ::eventBuffer::Muon_s[nElements];
   }
   // Wrapper around operator delete
   static void delete_eventBuffercLcLMuon_s(void *p) {
      delete (static_cast<::eventBuffer::Muon_s*>(p));
   }
   static void deleteArray_eventBuffercLcLMuon_s(void *p) {
      delete [] (static_cast<::eventBuffer::Muon_s*>(p));
   }
   static void destruct_eventBuffercLcLMuon_s(void *p) {
      typedef ::eventBuffer::Muon_s current_t;
      (static_cast<current_t*>(p))->~current_t();
   }
} // end of namespace ROOT for class ::eventBuffer::Muon_s

namespace ROOT {
   // Wrappers around operator new
   static void *new_eventBuffercLcLPhoton_s(void *p) {
      return  p ? ::new(static_cast<::ROOT::Internal::TOperatorNewHelper*>(p)) ::eventBuffer::Photon_s : new ::eventBuffer::Photon_s;
   }
   static void *newArray_eventBuffercLcLPhoton_s(Long_t nElements, void *p) {
      return p ? ::new(static_cast<::ROOT::Internal::TOperatorNewHelper*>(p)) ::eventBuffer::Photon_s[nElements] : new ::eventBuffer::Photon_s[nElements];
   }
   // Wrapper around operator delete
   static void delete_eventBuffercLcLPhoton_s(void *p) {
      delete (static_cast<::eventBuffer::Photon_s*>(p));
   }
   static void deleteArray_eventBuffercLcLPhoton_s(void *p) {
      delete [] (static_cast<::eventBuffer::Photon_s*>(p));
   }
   static void destruct_eventBuffercLcLPhoton_s(void *p) {
      typedef ::eventBuffer::Photon_s current_t;
      (static_cast<current_t*>(p))->~current_t();
   }
} // end of namespace ROOT for class ::eventBuffer::Photon_s

namespace ROOT {
   // Wrappers around operator new
   static void *new_eventBuffercLcLSubGenJetAK8_s(void *p) {
      return  p ? ::new(static_cast<::ROOT::Internal::TOperatorNewHelper*>(p)) ::eventBuffer::SubGenJetAK8_s : new ::eventBuffer::SubGenJetAK8_s;
   }
   static void *newArray_eventBuffercLcLSubGenJetAK8_s(Long_t nElements, void *p) {
      return p ? ::new(static_cast<::ROOT::Internal::TOperatorNewHelper*>(p)) ::eventBuffer::SubGenJetAK8_s[nElements] : new ::eventBuffer::SubGenJetAK8_s[nElements];
   }
   // Wrapper around operator delete
   static void delete_eventBuffercLcLSubGenJetAK8_s(void *p) {
      delete (static_cast<::eventBuffer::SubGenJetAK8_s*>(p));
   }
   static void deleteArray_eventBuffercLcLSubGenJetAK8_s(void *p) {
      delete [] (static_cast<::eventBuffer::SubGenJetAK8_s*>(p));
   }
   static void destruct_eventBuffercLcLSubGenJetAK8_s(void *p) {
      typedef ::eventBuffer::SubGenJetAK8_s current_t;
      (static_cast<current_t*>(p))->~current_t();
   }
} // end of namespace ROOT for class ::eventBuffer::SubGenJetAK8_s

namespace ROOT {
   // Wrappers around operator new
   static void *new_eventBuffercLcLSubJet_s(void *p) {
      return  p ? ::new(static_cast<::ROOT::Internal::TOperatorNewHelper*>(p)) ::eventBuffer::SubJet_s : new ::eventBuffer::SubJet_s;
   }
   static void *newArray_eventBuffercLcLSubJet_s(Long_t nElements, void *p) {
      return p ? ::new(static_cast<::ROOT::Internal::TOperatorNewHelper*>(p)) ::eventBuffer::SubJet_s[nElements] : new ::eventBuffer::SubJet_s[nElements];
   }
   // Wrapper around operator delete
   static void delete_eventBuffercLcLSubJet_s(void *p) {
      delete (static_cast<::eventBuffer::SubJet_s*>(p));
   }
   static void deleteArray_eventBuffercLcLSubJet_s(void *p) {
      delete [] (static_cast<::eventBuffer::SubJet_s*>(p));
   }
   static void destruct_eventBuffercLcLSubJet_s(void *p) {
      typedef ::eventBuffer::SubJet_s current_t;
      (static_cast<current_t*>(p))->~current_t();
   }
} // end of namespace ROOT for class ::eventBuffer::SubJet_s

namespace ROOT {
   // Wrappers around operator new
   static void *new_eventBuffercLcLTau_s(void *p) {
      return  p ? ::new(static_cast<::ROOT::Internal::TOperatorNewHelper*>(p)) ::eventBuffer::Tau_s : new ::eventBuffer::Tau_s;
   }
   static void *newArray_eventBuffercLcLTau_s(Long_t nElements, void *p) {
      return p ? ::new(static_cast<::ROOT::Internal::TOperatorNewHelper*>(p)) ::eventBuffer::Tau_s[nElements] : new ::eventBuffer::Tau_s[nElements];
   }
   // Wrapper around operator delete
   static void delete_eventBuffercLcLTau_s(void *p) {
      delete (static_cast<::eventBuffer::Tau_s*>(p));
   }
   static void deleteArray_eventBuffercLcLTau_s(void *p) {
      delete [] (static_cast<::eventBuffer::Tau_s*>(p));
   }
   static void destruct_eventBuffercLcLTau_s(void *p) {
      typedef ::eventBuffer::Tau_s current_t;
      (static_cast<current_t*>(p))->~current_t();
   }
} // end of namespace ROOT for class ::eventBuffer::Tau_s

namespace ROOT {
   // Wrappers around operator new
   static void *new_eventBuffercLcLTrigObj_s(void *p) {
      return  p ? ::new(static_cast<::ROOT::Internal::TOperatorNewHelper*>(p)) ::eventBuffer::TrigObj_s : new ::eventBuffer::TrigObj_s;
   }
   static void *newArray_eventBuffercLcLTrigObj_s(Long_t nElements, void *p) {
      return p ? ::new(static_cast<::ROOT::Internal::TOperatorNewHelper*>(p)) ::eventBuffer::TrigObj_s[nElements] : new ::eventBuffer::TrigObj_s[nElements];
   }
   // Wrapper around operator delete
   static void delete_eventBuffercLcLTrigObj_s(void *p) {
      delete (static_cast<::eventBuffer::TrigObj_s*>(p));
   }
   static void deleteArray_eventBuffercLcLTrigObj_s(void *p) {
      delete [] (static_cast<::eventBuffer::TrigObj_s*>(p));
   }
   static void destruct_eventBuffercLcLTrigObj_s(void *p) {
      typedef ::eventBuffer::TrigObj_s current_t;
      (static_cast<current_t*>(p))->~current_t();
   }
} // end of namespace ROOT for class ::eventBuffer::TrigObj_s

namespace ROOT {
   // Wrappers around operator new
   static void *new_eventBuffercLcLboostedTau_s(void *p) {
      return  p ? ::new(static_cast<::ROOT::Internal::TOperatorNewHelper*>(p)) ::eventBuffer::boostedTau_s : new ::eventBuffer::boostedTau_s;
   }
   static void *newArray_eventBuffercLcLboostedTau_s(Long_t nElements, void *p) {
      return p ? ::new(static_cast<::ROOT::Internal::TOperatorNewHelper*>(p)) ::eventBuffer::boostedTau_s[nElements] : new ::eventBuffer::boostedTau_s[nElements];
   }
   // Wrapper around operator delete
   static void delete_eventBuffercLcLboostedTau_s(void *p) {
      delete (static_cast<::eventBuffer::boostedTau_s*>(p));
   }
   static void deleteArray_eventBuffercLcLboostedTau_s(void *p) {
      delete [] (static_cast<::eventBuffer::boostedTau_s*>(p));
   }
   static void destruct_eventBuffercLcLboostedTau_s(void *p) {
      typedef ::eventBuffer::boostedTau_s current_t;
      (static_cast<current_t*>(p))->~current_t();
   }
} // end of namespace ROOT for class ::eventBuffer::boostedTau_s

namespace ROOT {
   // Wrapper around operator delete
   static void delete_outputFile(void *p) {
      delete (static_cast<::outputFile*>(p));
   }
   static void deleteArray_outputFile(void *p) {
      delete [] (static_cast<::outputFile*>(p));
   }
   static void destruct_outputFile(void *p) {
      typedef ::outputFile current_t;
      (static_cast<current_t*>(p))->~current_t();
   }
} // end of namespace ROOT for class ::outputFile

namespace ROOT {
   // Wrappers around operator new
   static void *new_commandLine(void *p) {
      return  p ? new(p) ::commandLine : new ::commandLine;
   }
   static void *newArray_commandLine(Long_t nElements, void *p) {
      return p ? new(p) ::commandLine[nElements] : new ::commandLine[nElements];
   }
   // Wrapper around operator delete
   static void delete_commandLine(void *p) {
      delete (static_cast<::commandLine*>(p));
   }
   static void deleteArray_commandLine(void *p) {
      delete [] (static_cast<::commandLine*>(p));
   }
   static void destruct_commandLine(void *p) {
      typedef ::commandLine current_t;
      (static_cast<current_t*>(p))->~current_t();
   }
} // end of namespace ROOT for class ::commandLine

namespace ROOT {
   // Wrappers around operator new
   static void *new_matchedPair(void *p) {
      return  p ? new(p) ::matchedPair : new ::matchedPair;
   }
   static void *newArray_matchedPair(Long_t nElements, void *p) {
      return p ? new(p) ::matchedPair[nElements] : new ::matchedPair[nElements];
   }
   // Wrapper around operator delete
   static void delete_matchedPair(void *p) {
      delete (static_cast<::matchedPair*>(p));
   }
   static void deleteArray_matchedPair(void *p) {
      delete [] (static_cast<::matchedPair*>(p));
   }
   static void destruct_matchedPair(void *p) {
      typedef ::matchedPair current_t;
      (static_cast<current_t*>(p))->~current_t();
   }
} // end of namespace ROOT for class ::matchedPair

namespace ROOT {
   // Wrappers around operator new
   static void *new_ptThing(void *p) {
      return  p ? new(p) ::ptThing : new ::ptThing;
   }
   static void *newArray_ptThing(Long_t nElements, void *p) {
      return p ? new(p) ::ptThing[nElements] : new ::ptThing[nElements];
   }
   // Wrapper around operator delete
   static void delete_ptThing(void *p) {
      delete (static_cast<::ptThing*>(p));
   }
   static void deleteArray_ptThing(void *p) {
      delete [] (static_cast<::ptThing*>(p));
   }
   static void destruct_ptThing(void *p) {
      typedef ::ptThing current_t;
      (static_cast<current_t*>(p))->~current_t();
   }
} // end of namespace ROOT for class ::ptThing

namespace ROOT {
   static TClass *vectorlEstringgR_Dictionary();
   static void vectorlEstringgR_TClassManip(TClass*);
   static void *new_vectorlEstringgR(void *p = nullptr);
   static void *newArray_vectorlEstringgR(Long_t size, void *p);
   static void delete_vectorlEstringgR(void *p);
   static void deleteArray_vectorlEstringgR(void *p);
   static void destruct_vectorlEstringgR(void *p);

   // Function generating the singleton type initializer
   static TGenericClassInfo *GenerateInitInstanceLocal(const vector<string>*)
   {
      vector<string> *ptr = nullptr;
      static ::TVirtualIsAProxy* isa_proxy = new ::TIsAProxy(typeid(vector<string>));
      static ::ROOT::TGenericClassInfo 
         instance("vector<string>", -2, "vector", 389,
                  typeid(vector<string>), ::ROOT::Internal::DefineBehavior(ptr, ptr),
                  &vectorlEstringgR_Dictionary, isa_proxy, 0,
                  sizeof(vector<string>) );
      instance.SetNew(&new_vectorlEstringgR);
      instance.SetNewArray(&newArray_vectorlEstringgR);
      instance.SetDelete(&delete_vectorlEstringgR);
      instance.SetDeleteArray(&deleteArray_vectorlEstringgR);
      instance.SetDestructor(&destruct_vectorlEstringgR);
      instance.AdoptCollectionProxyInfo(TCollectionProxyInfo::Generate(TCollectionProxyInfo::Pushback< vector<string> >()));

      instance.AdoptAlternate(::ROOT::AddClassAlternate("vector<string>","std::__1::vector<std::__1::basic_string<char, std::__1::char_traits<char>, std::__1::allocator<char>>, std::__1::allocator<std::__1::basic_string<char, std::__1::char_traits<char>, std::__1::allocator<char>>>>"));
      return &instance;
   }
   // Static variable to force the class initialization
   static ::ROOT::TGenericClassInfo *_R__UNIQUE_DICT_(Init) = GenerateInitInstanceLocal(static_cast<const vector<string>*>(nullptr)); R__UseDummy(_R__UNIQUE_DICT_(Init));

   // Dictionary for non-ClassDef classes
   static TClass *vectorlEstringgR_Dictionary() {
      TClass* theClass =::ROOT::GenerateInitInstanceLocal(static_cast<const vector<string>*>(nullptr))->GetClass();
      vectorlEstringgR_TClassManip(theClass);
   return theClass;
   }

   static void vectorlEstringgR_TClassManip(TClass* ){
   }

} // end of namespace ROOT

namespace ROOT {
   // Wrappers around operator new
   static void *new_vectorlEstringgR(void *p) {
      return  p ? ::new(static_cast<::ROOT::Internal::TOperatorNewHelper*>(p)) vector<string> : new vector<string>;
   }
   static void *newArray_vectorlEstringgR(Long_t nElements, void *p) {
      return p ? ::new(static_cast<::ROOT::Internal::TOperatorNewHelper*>(p)) vector<string>[nElements] : new vector<string>[nElements];
   }
   // Wrapper around operator delete
   static void delete_vectorlEstringgR(void *p) {
      delete (static_cast<vector<string>*>(p));
   }
   static void deleteArray_vectorlEstringgR(void *p) {
      delete [] (static_cast<vector<string>*>(p));
   }
   static void destruct_vectorlEstringgR(void *p) {
      typedef vector<string> current_t;
      (static_cast<current_t*>(p))->~current_t();
   }
} // end of namespace ROOT for class vector<string>

namespace ROOT {
   static TClass *vectorlEintgR_Dictionary();
   static void vectorlEintgR_TClassManip(TClass*);
   static void *new_vectorlEintgR(void *p = nullptr);
   static void *newArray_vectorlEintgR(Long_t size, void *p);
   static void delete_vectorlEintgR(void *p);
   static void deleteArray_vectorlEintgR(void *p);
   static void destruct_vectorlEintgR(void *p);

   // Function generating the singleton type initializer
   static TGenericClassInfo *GenerateInitInstanceLocal(const vector<int>*)
   {
      vector<int> *ptr = nullptr;
      static ::TVirtualIsAProxy* isa_proxy = new ::TIsAProxy(typeid(vector<int>));
      static ::ROOT::TGenericClassInfo 
         instance("vector<int>", -2, "vector", 389,
                  typeid(vector<int>), ::ROOT::Internal::DefineBehavior(ptr, ptr),
                  &vectorlEintgR_Dictionary, isa_proxy, 0,
                  sizeof(vector<int>) );
      instance.SetNew(&new_vectorlEintgR);
      instance.SetNewArray(&newArray_vectorlEintgR);
      instance.SetDelete(&delete_vectorlEintgR);
      instance.SetDeleteArray(&deleteArray_vectorlEintgR);
      instance.SetDestructor(&destruct_vectorlEintgR);
      instance.AdoptCollectionProxyInfo(TCollectionProxyInfo::Generate(TCollectionProxyInfo::Pushback< vector<int> >()));

      instance.AdoptAlternate(::ROOT::AddClassAlternate("vector<int>","std::__1::vector<int, std::__1::allocator<int>>"));
      return &instance;
   }
   // Static variable to force the class initialization
   static ::ROOT::TGenericClassInfo *_R__UNIQUE_DICT_(Init) = GenerateInitInstanceLocal(static_cast<const vector<int>*>(nullptr)); R__UseDummy(_R__UNIQUE_DICT_(Init));

   // Dictionary for non-ClassDef classes
   static TClass *vectorlEintgR_Dictionary() {
      TClass* theClass =::ROOT::GenerateInitInstanceLocal(static_cast<const vector<int>*>(nullptr))->GetClass();
      vectorlEintgR_TClassManip(theClass);
   return theClass;
   }

   static void vectorlEintgR_TClassManip(TClass* ){
   }

} // end of namespace ROOT

namespace ROOT {
   // Wrappers around operator new
   static void *new_vectorlEintgR(void *p) {
      return  p ? ::new(static_cast<::ROOT::Internal::TOperatorNewHelper*>(p)) vector<int> : new vector<int>;
   }
   static void *newArray_vectorlEintgR(Long_t nElements, void *p) {
      return p ? ::new(static_cast<::ROOT::Internal::TOperatorNewHelper*>(p)) vector<int>[nElements] : new vector<int>[nElements];
   }
   // Wrapper around operator delete
   static void delete_vectorlEintgR(void *p) {
      delete (static_cast<vector<int>*>(p));
   }
   static void deleteArray_vectorlEintgR(void *p) {
      delete [] (static_cast<vector<int>*>(p));
   }
   static void destruct_vectorlEintgR(void *p) {
      typedef vector<int> current_t;
      (static_cast<current_t*>(p))->~current_t();
   }
} // end of namespace ROOT for class vector<int>

namespace ROOT {
   static TClass *vectorlEintmUgR_Dictionary();
   static void vectorlEintmUgR_TClassManip(TClass*);
   static void *new_vectorlEintmUgR(void *p = nullptr);
   static void *newArray_vectorlEintmUgR(Long_t size, void *p);
   static void delete_vectorlEintmUgR(void *p);
   static void deleteArray_vectorlEintmUgR(void *p);
   static void destruct_vectorlEintmUgR(void *p);

   // Function generating the singleton type initializer
   static TGenericClassInfo *GenerateInitInstanceLocal(const vector<int*>*)
   {
      vector<int*> *ptr = nullptr;
      static ::TVirtualIsAProxy* isa_proxy = new ::TIsAProxy(typeid(vector<int*>));
      static ::ROOT::TGenericClassInfo 
         instance("vector<int*>", -2, "vector", 389,
                  typeid(vector<int*>), ::ROOT::Internal::DefineBehavior(ptr, ptr),
                  &vectorlEintmUgR_Dictionary, isa_proxy, 0,
                  sizeof(vector<int*>) );
      instance.SetNew(&new_vectorlEintmUgR);
      instance.SetNewArray(&newArray_vectorlEintmUgR);
      instance.SetDelete(&delete_vectorlEintmUgR);
      instance.SetDeleteArray(&deleteArray_vectorlEintmUgR);
      instance.SetDestructor(&destruct_vectorlEintmUgR);
      instance.AdoptCollectionProxyInfo(TCollectionProxyInfo::Generate(TCollectionProxyInfo::Pushback< vector<int*> >()));

      instance.AdoptAlternate(::ROOT::AddClassAlternate("vector<int*>","std::__1::vector<int*, std::__1::allocator<int*>>"));
      return &instance;
   }
   // Static variable to force the class initialization
   static ::ROOT::TGenericClassInfo *_R__UNIQUE_DICT_(Init) = GenerateInitInstanceLocal(static_cast<const vector<int*>*>(nullptr)); R__UseDummy(_R__UNIQUE_DICT_(Init));

   // Dictionary for non-ClassDef classes
   static TClass *vectorlEintmUgR_Dictionary() {
      TClass* theClass =::ROOT::GenerateInitInstanceLocal(static_cast<const vector<int*>*>(nullptr))->GetClass();
      vectorlEintmUgR_TClassManip(theClass);
   return theClass;
   }

   static void vectorlEintmUgR_TClassManip(TClass* ){
   }

} // end of namespace ROOT

namespace ROOT {
   // Wrappers around operator new
   static void *new_vectorlEintmUgR(void *p) {
      return  p ? ::new(static_cast<::ROOT::Internal::TOperatorNewHelper*>(p)) vector<int*> : new vector<int*>;
   }
   static void *newArray_vectorlEintmUgR(Long_t nElements, void *p) {
      return p ? ::new(static_cast<::ROOT::Internal::TOperatorNewHelper*>(p)) vector<int*>[nElements] : new vector<int*>[nElements];
   }
   // Wrapper around operator delete
   static void delete_vectorlEintmUgR(void *p) {
      delete (static_cast<vector<int*>*>(p));
   }
   static void deleteArray_vectorlEintmUgR(void *p) {
      delete [] (static_cast<vector<int*>*>(p));
   }
   static void destruct_vectorlEintmUgR(void *p) {
      typedef vector<int*> current_t;
      (static_cast<current_t*>(p))->~current_t();
   }
} // end of namespace ROOT for class vector<int*>

namespace ROOT {
   static TClass *vectorlEfloatgR_Dictionary();
   static void vectorlEfloatgR_TClassManip(TClass*);
   static void *new_vectorlEfloatgR(void *p = nullptr);
   static void *newArray_vectorlEfloatgR(Long_t size, void *p);
   static void delete_vectorlEfloatgR(void *p);
   static void deleteArray_vectorlEfloatgR(void *p);
   static void destruct_vectorlEfloatgR(void *p);

   // Function generating the singleton type initializer
   static TGenericClassInfo *GenerateInitInstanceLocal(const vector<float>*)
   {
      vector<float> *ptr = nullptr;
      static ::TVirtualIsAProxy* isa_proxy = new ::TIsAProxy(typeid(vector<float>));
      static ::ROOT::TGenericClassInfo 
         instance("vector<float>", -2, "vector", 389,
                  typeid(vector<float>), ::ROOT::Internal::DefineBehavior(ptr, ptr),
                  &vectorlEfloatgR_Dictionary, isa_proxy, 0,
                  sizeof(vector<float>) );
      instance.SetNew(&new_vectorlEfloatgR);
      instance.SetNewArray(&newArray_vectorlEfloatgR);
      instance.SetDelete(&delete_vectorlEfloatgR);
      instance.SetDeleteArray(&deleteArray_vectorlEfloatgR);
      instance.SetDestructor(&destruct_vectorlEfloatgR);
      instance.AdoptCollectionProxyInfo(TCollectionProxyInfo::Generate(TCollectionProxyInfo::Pushback< vector<float> >()));

      instance.AdoptAlternate(::ROOT::AddClassAlternate("vector<float>","std::__1::vector<float, std::__1::allocator<float>>"));
      return &instance;
   }
   // Static variable to force the class initialization
   static ::ROOT::TGenericClassInfo *_R__UNIQUE_DICT_(Init) = GenerateInitInstanceLocal(static_cast<const vector<float>*>(nullptr)); R__UseDummy(_R__UNIQUE_DICT_(Init));

   // Dictionary for non-ClassDef classes
   static TClass *vectorlEfloatgR_Dictionary() {
      TClass* theClass =::ROOT::GenerateInitInstanceLocal(static_cast<const vector<float>*>(nullptr))->GetClass();
      vectorlEfloatgR_TClassManip(theClass);
   return theClass;
   }

   static void vectorlEfloatgR_TClassManip(TClass* ){
   }

} // end of namespace ROOT

namespace ROOT {
   // Wrappers around operator new
   static void *new_vectorlEfloatgR(void *p) {
      return  p ? ::new(static_cast<::ROOT::Internal::TOperatorNewHelper*>(p)) vector<float> : new vector<float>;
   }
   static void *newArray_vectorlEfloatgR(Long_t nElements, void *p) {
      return p ? ::new(static_cast<::ROOT::Internal::TOperatorNewHelper*>(p)) vector<float>[nElements] : new vector<float>[nElements];
   }
   // Wrapper around operator delete
   static void delete_vectorlEfloatgR(void *p) {
      delete (static_cast<vector<float>*>(p));
   }
   static void deleteArray_vectorlEfloatgR(void *p) {
      delete [] (static_cast<vector<float>*>(p));
   }
   static void destruct_vectorlEfloatgR(void *p) {
      typedef vector<float> current_t;
      (static_cast<current_t*>(p))->~current_t();
   }
} // end of namespace ROOT for class vector<float>

namespace ROOT {
   static TClass *vectorlEeventBuffercLcLboostedTau_sgR_Dictionary();
   static void vectorlEeventBuffercLcLboostedTau_sgR_TClassManip(TClass*);
   static void *new_vectorlEeventBuffercLcLboostedTau_sgR(void *p = nullptr);
   static void *newArray_vectorlEeventBuffercLcLboostedTau_sgR(Long_t size, void *p);
   static void delete_vectorlEeventBuffercLcLboostedTau_sgR(void *p);
   static void deleteArray_vectorlEeventBuffercLcLboostedTau_sgR(void *p);
   static void destruct_vectorlEeventBuffercLcLboostedTau_sgR(void *p);

   // Function generating the singleton type initializer
   static TGenericClassInfo *GenerateInitInstanceLocal(const vector<eventBuffer::boostedTau_s>*)
   {
      vector<eventBuffer::boostedTau_s> *ptr = nullptr;
      static ::TVirtualIsAProxy* isa_proxy = new ::TIsAProxy(typeid(vector<eventBuffer::boostedTau_s>));
      static ::ROOT::TGenericClassInfo 
         instance("vector<eventBuffer::boostedTau_s>", -2, "vector", 389,
                  typeid(vector<eventBuffer::boostedTau_s>), ::ROOT::Internal::DefineBehavior(ptr, ptr),
                  &vectorlEeventBuffercLcLboostedTau_sgR_Dictionary, isa_proxy, 4,
                  sizeof(vector<eventBuffer::boostedTau_s>) );
      instance.SetNew(&new_vectorlEeventBuffercLcLboostedTau_sgR);
      instance.SetNewArray(&newArray_vectorlEeventBuffercLcLboostedTau_sgR);
      instance.SetDelete(&delete_vectorlEeventBuffercLcLboostedTau_sgR);
      instance.SetDeleteArray(&deleteArray_vectorlEeventBuffercLcLboostedTau_sgR);
      instance.SetDestructor(&destruct_vectorlEeventBuffercLcLboostedTau_sgR);
      instance.AdoptCollectionProxyInfo(TCollectionProxyInfo::Generate(TCollectionProxyInfo::Pushback< vector<eventBuffer::boostedTau_s> >()));

      instance.AdoptAlternate(::ROOT::AddClassAlternate("vector<eventBuffer::boostedTau_s>","std::__1::vector<eventBuffer::boostedTau_s, std::__1::allocator<eventBuffer::boostedTau_s>>"));
      return &instance;
   }
   // Static variable to force the class initialization
   static ::ROOT::TGenericClassInfo *_R__UNIQUE_DICT_(Init) = GenerateInitInstanceLocal(static_cast<const vector<eventBuffer::boostedTau_s>*>(nullptr)); R__UseDummy(_R__UNIQUE_DICT_(Init));

   // Dictionary for non-ClassDef classes
   static TClass *vectorlEeventBuffercLcLboostedTau_sgR_Dictionary() {
      TClass* theClass =::ROOT::GenerateInitInstanceLocal(static_cast<const vector<eventBuffer::boostedTau_s>*>(nullptr))->GetClass();
      vectorlEeventBuffercLcLboostedTau_sgR_TClassManip(theClass);
   return theClass;
   }

   static void vectorlEeventBuffercLcLboostedTau_sgR_TClassManip(TClass* ){
   }

} // end of namespace ROOT

namespace ROOT {
   // Wrappers around operator new
   static void *new_vectorlEeventBuffercLcLboostedTau_sgR(void *p) {
      return  p ? ::new(static_cast<::ROOT::Internal::TOperatorNewHelper*>(p)) vector<eventBuffer::boostedTau_s> : new vector<eventBuffer::boostedTau_s>;
   }
   static void *newArray_vectorlEeventBuffercLcLboostedTau_sgR(Long_t nElements, void *p) {
      return p ? ::new(static_cast<::ROOT::Internal::TOperatorNewHelper*>(p)) vector<eventBuffer::boostedTau_s>[nElements] : new vector<eventBuffer::boostedTau_s>[nElements];
   }
   // Wrapper around operator delete
   static void delete_vectorlEeventBuffercLcLboostedTau_sgR(void *p) {
      delete (static_cast<vector<eventBuffer::boostedTau_s>*>(p));
   }
   static void deleteArray_vectorlEeventBuffercLcLboostedTau_sgR(void *p) {
      delete [] (static_cast<vector<eventBuffer::boostedTau_s>*>(p));
   }
   static void destruct_vectorlEeventBuffercLcLboostedTau_sgR(void *p) {
      typedef vector<eventBuffer::boostedTau_s> current_t;
      (static_cast<current_t*>(p))->~current_t();
   }
} // end of namespace ROOT for class vector<eventBuffer::boostedTau_s>

namespace ROOT {
   static TClass *vectorlEeventBuffercLcLTrigObj_sgR_Dictionary();
   static void vectorlEeventBuffercLcLTrigObj_sgR_TClassManip(TClass*);
   static void *new_vectorlEeventBuffercLcLTrigObj_sgR(void *p = nullptr);
   static void *newArray_vectorlEeventBuffercLcLTrigObj_sgR(Long_t size, void *p);
   static void delete_vectorlEeventBuffercLcLTrigObj_sgR(void *p);
   static void deleteArray_vectorlEeventBuffercLcLTrigObj_sgR(void *p);
   static void destruct_vectorlEeventBuffercLcLTrigObj_sgR(void *p);

   // Function generating the singleton type initializer
   static TGenericClassInfo *GenerateInitInstanceLocal(const vector<eventBuffer::TrigObj_s>*)
   {
      vector<eventBuffer::TrigObj_s> *ptr = nullptr;
      static ::TVirtualIsAProxy* isa_proxy = new ::TIsAProxy(typeid(vector<eventBuffer::TrigObj_s>));
      static ::ROOT::TGenericClassInfo 
         instance("vector<eventBuffer::TrigObj_s>", -2, "vector", 389,
                  typeid(vector<eventBuffer::TrigObj_s>), ::ROOT::Internal::DefineBehavior(ptr, ptr),
                  &vectorlEeventBuffercLcLTrigObj_sgR_Dictionary, isa_proxy, 4,
                  sizeof(vector<eventBuffer::TrigObj_s>) );
      instance.SetNew(&new_vectorlEeventBuffercLcLTrigObj_sgR);
      instance.SetNewArray(&newArray_vectorlEeventBuffercLcLTrigObj_sgR);
      instance.SetDelete(&delete_vectorlEeventBuffercLcLTrigObj_sgR);
      instance.SetDeleteArray(&deleteArray_vectorlEeventBuffercLcLTrigObj_sgR);
      instance.SetDestructor(&destruct_vectorlEeventBuffercLcLTrigObj_sgR);
      instance.AdoptCollectionProxyInfo(TCollectionProxyInfo::Generate(TCollectionProxyInfo::Pushback< vector<eventBuffer::TrigObj_s> >()));

      instance.AdoptAlternate(::ROOT::AddClassAlternate("vector<eventBuffer::TrigObj_s>","std::__1::vector<eventBuffer::TrigObj_s, std::__1::allocator<eventBuffer::TrigObj_s>>"));
      return &instance;
   }
   // Static variable to force the class initialization
   static ::ROOT::TGenericClassInfo *_R__UNIQUE_DICT_(Init) = GenerateInitInstanceLocal(static_cast<const vector<eventBuffer::TrigObj_s>*>(nullptr)); R__UseDummy(_R__UNIQUE_DICT_(Init));

   // Dictionary for non-ClassDef classes
   static TClass *vectorlEeventBuffercLcLTrigObj_sgR_Dictionary() {
      TClass* theClass =::ROOT::GenerateInitInstanceLocal(static_cast<const vector<eventBuffer::TrigObj_s>*>(nullptr))->GetClass();
      vectorlEeventBuffercLcLTrigObj_sgR_TClassManip(theClass);
   return theClass;
   }

   static void vectorlEeventBuffercLcLTrigObj_sgR_TClassManip(TClass* ){
   }

} // end of namespace ROOT

namespace ROOT {
   // Wrappers around operator new
   static void *new_vectorlEeventBuffercLcLTrigObj_sgR(void *p) {
      return  p ? ::new(static_cast<::ROOT::Internal::TOperatorNewHelper*>(p)) vector<eventBuffer::TrigObj_s> : new vector<eventBuffer::TrigObj_s>;
   }
   static void *newArray_vectorlEeventBuffercLcLTrigObj_sgR(Long_t nElements, void *p) {
      return p ? ::new(static_cast<::ROOT::Internal::TOperatorNewHelper*>(p)) vector<eventBuffer::TrigObj_s>[nElements] : new vector<eventBuffer::TrigObj_s>[nElements];
   }
   // Wrapper around operator delete
   static void delete_vectorlEeventBuffercLcLTrigObj_sgR(void *p) {
      delete (static_cast<vector<eventBuffer::TrigObj_s>*>(p));
   }
   static void deleteArray_vectorlEeventBuffercLcLTrigObj_sgR(void *p) {
      delete [] (static_cast<vector<eventBuffer::TrigObj_s>*>(p));
   }
   static void destruct_vectorlEeventBuffercLcLTrigObj_sgR(void *p) {
      typedef vector<eventBuffer::TrigObj_s> current_t;
      (static_cast<current_t*>(p))->~current_t();
   }
} // end of namespace ROOT for class vector<eventBuffer::TrigObj_s>

namespace ROOT {
   static TClass *vectorlEeventBuffercLcLTau_sgR_Dictionary();
   static void vectorlEeventBuffercLcLTau_sgR_TClassManip(TClass*);
   static void *new_vectorlEeventBuffercLcLTau_sgR(void *p = nullptr);
   static void *newArray_vectorlEeventBuffercLcLTau_sgR(Long_t size, void *p);
   static void delete_vectorlEeventBuffercLcLTau_sgR(void *p);
   static void deleteArray_vectorlEeventBuffercLcLTau_sgR(void *p);
   static void destruct_vectorlEeventBuffercLcLTau_sgR(void *p);

   // Function generating the singleton type initializer
   static TGenericClassInfo *GenerateInitInstanceLocal(const vector<eventBuffer::Tau_s>*)
   {
      vector<eventBuffer::Tau_s> *ptr = nullptr;
      static ::TVirtualIsAProxy* isa_proxy = new ::TIsAProxy(typeid(vector<eventBuffer::Tau_s>));
      static ::ROOT::TGenericClassInfo 
         instance("vector<eventBuffer::Tau_s>", -2, "vector", 389,
                  typeid(vector<eventBuffer::Tau_s>), ::ROOT::Internal::DefineBehavior(ptr, ptr),
                  &vectorlEeventBuffercLcLTau_sgR_Dictionary, isa_proxy, 4,
                  sizeof(vector<eventBuffer::Tau_s>) );
      instance.SetNew(&new_vectorlEeventBuffercLcLTau_sgR);
      instance.SetNewArray(&newArray_vectorlEeventBuffercLcLTau_sgR);
      instance.SetDelete(&delete_vectorlEeventBuffercLcLTau_sgR);
      instance.SetDeleteArray(&deleteArray_vectorlEeventBuffercLcLTau_sgR);
      instance.SetDestructor(&destruct_vectorlEeventBuffercLcLTau_sgR);
      instance.AdoptCollectionProxyInfo(TCollectionProxyInfo::Generate(TCollectionProxyInfo::Pushback< vector<eventBuffer::Tau_s> >()));

      instance.AdoptAlternate(::ROOT::AddClassAlternate("vector<eventBuffer::Tau_s>","std::__1::vector<eventBuffer::Tau_s, std::__1::allocator<eventBuffer::Tau_s>>"));
      return &instance;
   }
   // Static variable to force the class initialization
   static ::ROOT::TGenericClassInfo *_R__UNIQUE_DICT_(Init) = GenerateInitInstanceLocal(static_cast<const vector<eventBuffer::Tau_s>*>(nullptr)); R__UseDummy(_R__UNIQUE_DICT_(Init));

   // Dictionary for non-ClassDef classes
   static TClass *vectorlEeventBuffercLcLTau_sgR_Dictionary() {
      TClass* theClass =::ROOT::GenerateInitInstanceLocal(static_cast<const vector<eventBuffer::Tau_s>*>(nullptr))->GetClass();
      vectorlEeventBuffercLcLTau_sgR_TClassManip(theClass);
   return theClass;
   }

   static void vectorlEeventBuffercLcLTau_sgR_TClassManip(TClass* ){
   }

} // end of namespace ROOT

namespace ROOT {
   // Wrappers around operator new
   static void *new_vectorlEeventBuffercLcLTau_sgR(void *p) {
      return  p ? ::new(static_cast<::ROOT::Internal::TOperatorNewHelper*>(p)) vector<eventBuffer::Tau_s> : new vector<eventBuffer::Tau_s>;
   }
   static void *newArray_vectorlEeventBuffercLcLTau_sgR(Long_t nElements, void *p) {
      return p ? ::new(static_cast<::ROOT::Internal::TOperatorNewHelper*>(p)) vector<eventBuffer::Tau_s>[nElements] : new vector<eventBuffer::Tau_s>[nElements];
   }
   // Wrapper around operator delete
   static void delete_vectorlEeventBuffercLcLTau_sgR(void *p) {
      delete (static_cast<vector<eventBuffer::Tau_s>*>(p));
   }
   static void deleteArray_vectorlEeventBuffercLcLTau_sgR(void *p) {
      delete [] (static_cast<vector<eventBuffer::Tau_s>*>(p));
   }
   static void destruct_vectorlEeventBuffercLcLTau_sgR(void *p) {
      typedef vector<eventBuffer::Tau_s> current_t;
      (static_cast<current_t*>(p))->~current_t();
   }
} // end of namespace ROOT for class vector<eventBuffer::Tau_s>

namespace ROOT {
   static TClass *vectorlEeventBuffercLcLSubJet_sgR_Dictionary();
   static void vectorlEeventBuffercLcLSubJet_sgR_TClassManip(TClass*);
   static void *new_vectorlEeventBuffercLcLSubJet_sgR(void *p = nullptr);
   static void *newArray_vectorlEeventBuffercLcLSubJet_sgR(Long_t size, void *p);
   static void delete_vectorlEeventBuffercLcLSubJet_sgR(void *p);
   static void deleteArray_vectorlEeventBuffercLcLSubJet_sgR(void *p);
   static void destruct_vectorlEeventBuffercLcLSubJet_sgR(void *p);

   // Function generating the singleton type initializer
   static TGenericClassInfo *GenerateInitInstanceLocal(const vector<eventBuffer::SubJet_s>*)
   {
      vector<eventBuffer::SubJet_s> *ptr = nullptr;
      static ::TVirtualIsAProxy* isa_proxy = new ::TIsAProxy(typeid(vector<eventBuffer::SubJet_s>));
      static ::ROOT::TGenericClassInfo 
         instance("vector<eventBuffer::SubJet_s>", -2, "vector", 389,
                  typeid(vector<eventBuffer::SubJet_s>), ::ROOT::Internal::DefineBehavior(ptr, ptr),
                  &vectorlEeventBuffercLcLSubJet_sgR_Dictionary, isa_proxy, 4,
                  sizeof(vector<eventBuffer::SubJet_s>) );
      instance.SetNew(&new_vectorlEeventBuffercLcLSubJet_sgR);
      instance.SetNewArray(&newArray_vectorlEeventBuffercLcLSubJet_sgR);
      instance.SetDelete(&delete_vectorlEeventBuffercLcLSubJet_sgR);
      instance.SetDeleteArray(&deleteArray_vectorlEeventBuffercLcLSubJet_sgR);
      instance.SetDestructor(&destruct_vectorlEeventBuffercLcLSubJet_sgR);
      instance.AdoptCollectionProxyInfo(TCollectionProxyInfo::Generate(TCollectionProxyInfo::Pushback< vector<eventBuffer::SubJet_s> >()));

      instance.AdoptAlternate(::ROOT::AddClassAlternate("vector<eventBuffer::SubJet_s>","std::__1::vector<eventBuffer::SubJet_s, std::__1::allocator<eventBuffer::SubJet_s>>"));
      return &instance;
   }
   // Static variable to force the class initialization
   static ::ROOT::TGenericClassInfo *_R__UNIQUE_DICT_(Init) = GenerateInitInstanceLocal(static_cast<const vector<eventBuffer::SubJet_s>*>(nullptr)); R__UseDummy(_R__UNIQUE_DICT_(Init));

   // Dictionary for non-ClassDef classes
   static TClass *vectorlEeventBuffercLcLSubJet_sgR_Dictionary() {
      TClass* theClass =::ROOT::GenerateInitInstanceLocal(static_cast<const vector<eventBuffer::SubJet_s>*>(nullptr))->GetClass();
      vectorlEeventBuffercLcLSubJet_sgR_TClassManip(theClass);
   return theClass;
   }

   static void vectorlEeventBuffercLcLSubJet_sgR_TClassManip(TClass* ){
   }

} // end of namespace ROOT

namespace ROOT {
   // Wrappers around operator new
   static void *new_vectorlEeventBuffercLcLSubJet_sgR(void *p) {
      return  p ? ::new(static_cast<::ROOT::Internal::TOperatorNewHelper*>(p)) vector<eventBuffer::SubJet_s> : new vector<eventBuffer::SubJet_s>;
   }
   static void *newArray_vectorlEeventBuffercLcLSubJet_sgR(Long_t nElements, void *p) {
      return p ? ::new(static_cast<::ROOT::Internal::TOperatorNewHelper*>(p)) vector<eventBuffer::SubJet_s>[nElements] : new vector<eventBuffer::SubJet_s>[nElements];
   }
   // Wrapper around operator delete
   static void delete_vectorlEeventBuffercLcLSubJet_sgR(void *p) {
      delete (static_cast<vector<eventBuffer::SubJet_s>*>(p));
   }
   static void deleteArray_vectorlEeventBuffercLcLSubJet_sgR(void *p) {
      delete [] (static_cast<vector<eventBuffer::SubJet_s>*>(p));
   }
   static void destruct_vectorlEeventBuffercLcLSubJet_sgR(void *p) {
      typedef vector<eventBuffer::SubJet_s> current_t;
      (static_cast<current_t*>(p))->~current_t();
   }
} // end of namespace ROOT for class vector<eventBuffer::SubJet_s>

namespace ROOT {
   static TClass *vectorlEeventBuffercLcLSubGenJetAK8_sgR_Dictionary();
   static void vectorlEeventBuffercLcLSubGenJetAK8_sgR_TClassManip(TClass*);
   static void *new_vectorlEeventBuffercLcLSubGenJetAK8_sgR(void *p = nullptr);
   static void *newArray_vectorlEeventBuffercLcLSubGenJetAK8_sgR(Long_t size, void *p);
   static void delete_vectorlEeventBuffercLcLSubGenJetAK8_sgR(void *p);
   static void deleteArray_vectorlEeventBuffercLcLSubGenJetAK8_sgR(void *p);
   static void destruct_vectorlEeventBuffercLcLSubGenJetAK8_sgR(void *p);

   // Function generating the singleton type initializer
   static TGenericClassInfo *GenerateInitInstanceLocal(const vector<eventBuffer::SubGenJetAK8_s>*)
   {
      vector<eventBuffer::SubGenJetAK8_s> *ptr = nullptr;
      static ::TVirtualIsAProxy* isa_proxy = new ::TIsAProxy(typeid(vector<eventBuffer::SubGenJetAK8_s>));
      static ::ROOT::TGenericClassInfo 
         instance("vector<eventBuffer::SubGenJetAK8_s>", -2, "vector", 389,
                  typeid(vector<eventBuffer::SubGenJetAK8_s>), ::ROOT::Internal::DefineBehavior(ptr, ptr),
                  &vectorlEeventBuffercLcLSubGenJetAK8_sgR_Dictionary, isa_proxy, 4,
                  sizeof(vector<eventBuffer::SubGenJetAK8_s>) );
      instance.SetNew(&new_vectorlEeventBuffercLcLSubGenJetAK8_sgR);
      instance.SetNewArray(&newArray_vectorlEeventBuffercLcLSubGenJetAK8_sgR);
      instance.SetDelete(&delete_vectorlEeventBuffercLcLSubGenJetAK8_sgR);
      instance.SetDeleteArray(&deleteArray_vectorlEeventBuffercLcLSubGenJetAK8_sgR);
      instance.SetDestructor(&destruct_vectorlEeventBuffercLcLSubGenJetAK8_sgR);
      instance.AdoptCollectionProxyInfo(TCollectionProxyInfo::Generate(TCollectionProxyInfo::Pushback< vector<eventBuffer::SubGenJetAK8_s> >()));

      instance.AdoptAlternate(::ROOT::AddClassAlternate("vector<eventBuffer::SubGenJetAK8_s>","std::__1::vector<eventBuffer::SubGenJetAK8_s, std::__1::allocator<eventBuffer::SubGenJetAK8_s>>"));
      return &instance;
   }
   // Static variable to force the class initialization
   static ::ROOT::TGenericClassInfo *_R__UNIQUE_DICT_(Init) = GenerateInitInstanceLocal(static_cast<const vector<eventBuffer::SubGenJetAK8_s>*>(nullptr)); R__UseDummy(_R__UNIQUE_DICT_(Init));

   // Dictionary for non-ClassDef classes
   static TClass *vectorlEeventBuffercLcLSubGenJetAK8_sgR_Dictionary() {
      TClass* theClass =::ROOT::GenerateInitInstanceLocal(static_cast<const vector<eventBuffer::SubGenJetAK8_s>*>(nullptr))->GetClass();
      vectorlEeventBuffercLcLSubGenJetAK8_sgR_TClassManip(theClass);
   return theClass;
   }

   static void vectorlEeventBuffercLcLSubGenJetAK8_sgR_TClassManip(TClass* ){
   }

} // end of namespace ROOT

namespace ROOT {
   // Wrappers around operator new
   static void *new_vectorlEeventBuffercLcLSubGenJetAK8_sgR(void *p) {
      return  p ? ::new(static_cast<::ROOT::Internal::TOperatorNewHelper*>(p)) vector<eventBuffer::SubGenJetAK8_s> : new vector<eventBuffer::SubGenJetAK8_s>;
   }
   static void *newArray_vectorlEeventBuffercLcLSubGenJetAK8_sgR(Long_t nElements, void *p) {
      return p ? ::new(static_cast<::ROOT::Internal::TOperatorNewHelper*>(p)) vector<eventBuffer::SubGenJetAK8_s>[nElements] : new vector<eventBuffer::SubGenJetAK8_s>[nElements];
   }
   // Wrapper around operator delete
   static void delete_vectorlEeventBuffercLcLSubGenJetAK8_sgR(void *p) {
      delete (static_cast<vector<eventBuffer::SubGenJetAK8_s>*>(p));
   }
   static void deleteArray_vectorlEeventBuffercLcLSubGenJetAK8_sgR(void *p) {
      delete [] (static_cast<vector<eventBuffer::SubGenJetAK8_s>*>(p));
   }
   static void destruct_vectorlEeventBuffercLcLSubGenJetAK8_sgR(void *p) {
      typedef vector<eventBuffer::SubGenJetAK8_s> current_t;
      (static_cast<current_t*>(p))->~current_t();
   }
} // end of namespace ROOT for class vector<eventBuffer::SubGenJetAK8_s>

namespace ROOT {
   static TClass *vectorlEeventBuffercLcLPhoton_sgR_Dictionary();
   static void vectorlEeventBuffercLcLPhoton_sgR_TClassManip(TClass*);
   static void *new_vectorlEeventBuffercLcLPhoton_sgR(void *p = nullptr);
   static void *newArray_vectorlEeventBuffercLcLPhoton_sgR(Long_t size, void *p);
   static void delete_vectorlEeventBuffercLcLPhoton_sgR(void *p);
   static void deleteArray_vectorlEeventBuffercLcLPhoton_sgR(void *p);
   static void destruct_vectorlEeventBuffercLcLPhoton_sgR(void *p);

   // Function generating the singleton type initializer
   static TGenericClassInfo *GenerateInitInstanceLocal(const vector<eventBuffer::Photon_s>*)
   {
      vector<eventBuffer::Photon_s> *ptr = nullptr;
      static ::TVirtualIsAProxy* isa_proxy = new ::TIsAProxy(typeid(vector<eventBuffer::Photon_s>));
      static ::ROOT::TGenericClassInfo 
         instance("vector<eventBuffer::Photon_s>", -2, "vector", 389,
                  typeid(vector<eventBuffer::Photon_s>), ::ROOT::Internal::DefineBehavior(ptr, ptr),
                  &vectorlEeventBuffercLcLPhoton_sgR_Dictionary, isa_proxy, 4,
                  sizeof(vector<eventBuffer::Photon_s>) );
      instance.SetNew(&new_vectorlEeventBuffercLcLPhoton_sgR);
      instance.SetNewArray(&newArray_vectorlEeventBuffercLcLPhoton_sgR);
      instance.SetDelete(&delete_vectorlEeventBuffercLcLPhoton_sgR);
      instance.SetDeleteArray(&deleteArray_vectorlEeventBuffercLcLPhoton_sgR);
      instance.SetDestructor(&destruct_vectorlEeventBuffercLcLPhoton_sgR);
      instance.AdoptCollectionProxyInfo(TCollectionProxyInfo::Generate(TCollectionProxyInfo::Pushback< vector<eventBuffer::Photon_s> >()));

      instance.AdoptAlternate(::ROOT::AddClassAlternate("vector<eventBuffer::Photon_s>","std::__1::vector<eventBuffer::Photon_s, std::__1::allocator<eventBuffer::Photon_s>>"));
      return &instance;
   }
   // Static variable to force the class initialization
   static ::ROOT::TGenericClassInfo *_R__UNIQUE_DICT_(Init) = GenerateInitInstanceLocal(static_cast<const vector<eventBuffer::Photon_s>*>(nullptr)); R__UseDummy(_R__UNIQUE_DICT_(Init));

   // Dictionary for non-ClassDef classes
   static TClass *vectorlEeventBuffercLcLPhoton_sgR_Dictionary() {
      TClass* theClass =::ROOT::GenerateInitInstanceLocal(static_cast<const vector<eventBuffer::Photon_s>*>(nullptr))->GetClass();
      vectorlEeventBuffercLcLPhoton_sgR_TClassManip(theClass);
   return theClass;
   }

   static void vectorlEeventBuffercLcLPhoton_sgR_TClassManip(TClass* ){
   }

} // end of namespace ROOT

namespace ROOT {
   // Wrappers around operator new
   static void *new_vectorlEeventBuffercLcLPhoton_sgR(void *p) {
      return  p ? ::new(static_cast<::ROOT::Internal::TOperatorNewHelper*>(p)) vector<eventBuffer::Photon_s> : new vector<eventBuffer::Photon_s>;
   }
   static void *newArray_vectorlEeventBuffercLcLPhoton_sgR(Long_t nElements, void *p) {
      return p ? ::new(static_cast<::ROOT::Internal::TOperatorNewHelper*>(p)) vector<eventBuffer::Photon_s>[nElements] : new vector<eventBuffer::Photon_s>[nElements];
   }
   // Wrapper around operator delete
   static void delete_vectorlEeventBuffercLcLPhoton_sgR(void *p) {
      delete (static_cast<vector<eventBuffer::Photon_s>*>(p));
   }
   static void deleteArray_vectorlEeventBuffercLcLPhoton_sgR(void *p) {
      delete [] (static_cast<vector<eventBuffer::Photon_s>*>(p));
   }
   static void destruct_vectorlEeventBuffercLcLPhoton_sgR(void *p) {
      typedef vector<eventBuffer::Photon_s> current_t;
      (static_cast<current_t*>(p))->~current_t();
   }
} // end of namespace ROOT for class vector<eventBuffer::Photon_s>

namespace ROOT {
   static TClass *vectorlEeventBuffercLcLMuon_sgR_Dictionary();
   static void vectorlEeventBuffercLcLMuon_sgR_TClassManip(TClass*);
   static void *new_vectorlEeventBuffercLcLMuon_sgR(void *p = nullptr);
   static void *newArray_vectorlEeventBuffercLcLMuon_sgR(Long_t size, void *p);
   static void delete_vectorlEeventBuffercLcLMuon_sgR(void *p);
   static void deleteArray_vectorlEeventBuffercLcLMuon_sgR(void *p);
   static void destruct_vectorlEeventBuffercLcLMuon_sgR(void *p);

   // Function generating the singleton type initializer
   static TGenericClassInfo *GenerateInitInstanceLocal(const vector<eventBuffer::Muon_s>*)
   {
      vector<eventBuffer::Muon_s> *ptr = nullptr;
      static ::TVirtualIsAProxy* isa_proxy = new ::TIsAProxy(typeid(vector<eventBuffer::Muon_s>));
      static ::ROOT::TGenericClassInfo 
         instance("vector<eventBuffer::Muon_s>", -2, "vector", 389,
                  typeid(vector<eventBuffer::Muon_s>), ::ROOT::Internal::DefineBehavior(ptr, ptr),
                  &vectorlEeventBuffercLcLMuon_sgR_Dictionary, isa_proxy, 4,
                  sizeof(vector<eventBuffer::Muon_s>) );
      instance.SetNew(&new_vectorlEeventBuffercLcLMuon_sgR);
      instance.SetNewArray(&newArray_vectorlEeventBuffercLcLMuon_sgR);
      instance.SetDelete(&delete_vectorlEeventBuffercLcLMuon_sgR);
      instance.SetDeleteArray(&deleteArray_vectorlEeventBuffercLcLMuon_sgR);
      instance.SetDestructor(&destruct_vectorlEeventBuffercLcLMuon_sgR);
      instance.AdoptCollectionProxyInfo(TCollectionProxyInfo::Generate(TCollectionProxyInfo::Pushback< vector<eventBuffer::Muon_s> >()));

      instance.AdoptAlternate(::ROOT::AddClassAlternate("vector<eventBuffer::Muon_s>","std::__1::vector<eventBuffer::Muon_s, std::__1::allocator<eventBuffer::Muon_s>>"));
      return &instance;
   }
   // Static variable to force the class initialization
   static ::ROOT::TGenericClassInfo *_R__UNIQUE_DICT_(Init) = GenerateInitInstanceLocal(static_cast<const vector<eventBuffer::Muon_s>*>(nullptr)); R__UseDummy(_R__UNIQUE_DICT_(Init));

   // Dictionary for non-ClassDef classes
   static TClass *vectorlEeventBuffercLcLMuon_sgR_Dictionary() {
      TClass* theClass =::ROOT::GenerateInitInstanceLocal(static_cast<const vector<eventBuffer::Muon_s>*>(nullptr))->GetClass();
      vectorlEeventBuffercLcLMuon_sgR_TClassManip(theClass);
   return theClass;
   }

   static void vectorlEeventBuffercLcLMuon_sgR_TClassManip(TClass* ){
   }

} // end of namespace ROOT

namespace ROOT {
   // Wrappers around operator new
   static void *new_vectorlEeventBuffercLcLMuon_sgR(void *p) {
      return  p ? ::new(static_cast<::ROOT::Internal::TOperatorNewHelper*>(p)) vector<eventBuffer::Muon_s> : new vector<eventBuffer::Muon_s>;
   }
   static void *newArray_vectorlEeventBuffercLcLMuon_sgR(Long_t nElements, void *p) {
      return p ? ::new(static_cast<::ROOT::Internal::TOperatorNewHelper*>(p)) vector<eventBuffer::Muon_s>[nElements] : new vector<eventBuffer::Muon_s>[nElements];
   }
   // Wrapper around operator delete
   static void delete_vectorlEeventBuffercLcLMuon_sgR(void *p) {
      delete (static_cast<vector<eventBuffer::Muon_s>*>(p));
   }
   static void deleteArray_vectorlEeventBuffercLcLMuon_sgR(void *p) {
      delete [] (static_cast<vector<eventBuffer::Muon_s>*>(p));
   }
   static void destruct_vectorlEeventBuffercLcLMuon_sgR(void *p) {
      typedef vector<eventBuffer::Muon_s> current_t;
      (static_cast<current_t*>(p))->~current_t();
   }
} // end of namespace ROOT for class vector<eventBuffer::Muon_s>

namespace ROOT {
   static TClass *vectorlEeventBuffercLcLLHEPart_sgR_Dictionary();
   static void vectorlEeventBuffercLcLLHEPart_sgR_TClassManip(TClass*);
   static void *new_vectorlEeventBuffercLcLLHEPart_sgR(void *p = nullptr);
   static void *newArray_vectorlEeventBuffercLcLLHEPart_sgR(Long_t size, void *p);
   static void delete_vectorlEeventBuffercLcLLHEPart_sgR(void *p);
   static void deleteArray_vectorlEeventBuffercLcLLHEPart_sgR(void *p);
   static void destruct_vectorlEeventBuffercLcLLHEPart_sgR(void *p);

   // Function generating the singleton type initializer
   static TGenericClassInfo *GenerateInitInstanceLocal(const vector<eventBuffer::LHEPart_s>*)
   {
      vector<eventBuffer::LHEPart_s> *ptr = nullptr;
      static ::TVirtualIsAProxy* isa_proxy = new ::TIsAProxy(typeid(vector<eventBuffer::LHEPart_s>));
      static ::ROOT::TGenericClassInfo 
         instance("vector<eventBuffer::LHEPart_s>", -2, "vector", 389,
                  typeid(vector<eventBuffer::LHEPart_s>), ::ROOT::Internal::DefineBehavior(ptr, ptr),
                  &vectorlEeventBuffercLcLLHEPart_sgR_Dictionary, isa_proxy, 4,
                  sizeof(vector<eventBuffer::LHEPart_s>) );
      instance.SetNew(&new_vectorlEeventBuffercLcLLHEPart_sgR);
      instance.SetNewArray(&newArray_vectorlEeventBuffercLcLLHEPart_sgR);
      instance.SetDelete(&delete_vectorlEeventBuffercLcLLHEPart_sgR);
      instance.SetDeleteArray(&deleteArray_vectorlEeventBuffercLcLLHEPart_sgR);
      instance.SetDestructor(&destruct_vectorlEeventBuffercLcLLHEPart_sgR);
      instance.AdoptCollectionProxyInfo(TCollectionProxyInfo::Generate(TCollectionProxyInfo::Pushback< vector<eventBuffer::LHEPart_s> >()));

      instance.AdoptAlternate(::ROOT::AddClassAlternate("vector<eventBuffer::LHEPart_s>","std::__1::vector<eventBuffer::LHEPart_s, std::__1::allocator<eventBuffer::LHEPart_s>>"));
      return &instance;
   }
   // Static variable to force the class initialization
   static ::ROOT::TGenericClassInfo *_R__UNIQUE_DICT_(Init) = GenerateInitInstanceLocal(static_cast<const vector<eventBuffer::LHEPart_s>*>(nullptr)); R__UseDummy(_R__UNIQUE_DICT_(Init));

   // Dictionary for non-ClassDef classes
   static TClass *vectorlEeventBuffercLcLLHEPart_sgR_Dictionary() {
      TClass* theClass =::ROOT::GenerateInitInstanceLocal(static_cast<const vector<eventBuffer::LHEPart_s>*>(nullptr))->GetClass();
      vectorlEeventBuffercLcLLHEPart_sgR_TClassManip(theClass);
   return theClass;
   }

   static void vectorlEeventBuffercLcLLHEPart_sgR_TClassManip(TClass* ){
   }

} // end of namespace ROOT

namespace ROOT {
   // Wrappers around operator new
   static void *new_vectorlEeventBuffercLcLLHEPart_sgR(void *p) {
      return  p ? ::new(static_cast<::ROOT::Internal::TOperatorNewHelper*>(p)) vector<eventBuffer::LHEPart_s> : new vector<eventBuffer::LHEPart_s>;
   }
   static void *newArray_vectorlEeventBuffercLcLLHEPart_sgR(Long_t nElements, void *p) {
      return p ? ::new(static_cast<::ROOT::Internal::TOperatorNewHelper*>(p)) vector<eventBuffer::LHEPart_s>[nElements] : new vector<eventBuffer::LHEPart_s>[nElements];
   }
   // Wrapper around operator delete
   static void delete_vectorlEeventBuffercLcLLHEPart_sgR(void *p) {
      delete (static_cast<vector<eventBuffer::LHEPart_s>*>(p));
   }
   static void deleteArray_vectorlEeventBuffercLcLLHEPart_sgR(void *p) {
      delete [] (static_cast<vector<eventBuffer::LHEPart_s>*>(p));
   }
   static void destruct_vectorlEeventBuffercLcLLHEPart_sgR(void *p) {
      typedef vector<eventBuffer::LHEPart_s> current_t;
      (static_cast<current_t*>(p))->~current_t();
   }
} // end of namespace ROOT for class vector<eventBuffer::LHEPart_s>

namespace ROOT {
   static TClass *vectorlEeventBuffercLcLJet_sgR_Dictionary();
   static void vectorlEeventBuffercLcLJet_sgR_TClassManip(TClass*);
   static void *new_vectorlEeventBuffercLcLJet_sgR(void *p = nullptr);
   static void *newArray_vectorlEeventBuffercLcLJet_sgR(Long_t size, void *p);
   static void delete_vectorlEeventBuffercLcLJet_sgR(void *p);
   static void deleteArray_vectorlEeventBuffercLcLJet_sgR(void *p);
   static void destruct_vectorlEeventBuffercLcLJet_sgR(void *p);

   // Function generating the singleton type initializer
   static TGenericClassInfo *GenerateInitInstanceLocal(const vector<eventBuffer::Jet_s>*)
   {
      vector<eventBuffer::Jet_s> *ptr = nullptr;
      static ::TVirtualIsAProxy* isa_proxy = new ::TIsAProxy(typeid(vector<eventBuffer::Jet_s>));
      static ::ROOT::TGenericClassInfo 
         instance("vector<eventBuffer::Jet_s>", -2, "vector", 389,
                  typeid(vector<eventBuffer::Jet_s>), ::ROOT::Internal::DefineBehavior(ptr, ptr),
                  &vectorlEeventBuffercLcLJet_sgR_Dictionary, isa_proxy, 4,
                  sizeof(vector<eventBuffer::Jet_s>) );
      instance.SetNew(&new_vectorlEeventBuffercLcLJet_sgR);
      instance.SetNewArray(&newArray_vectorlEeventBuffercLcLJet_sgR);
      instance.SetDelete(&delete_vectorlEeventBuffercLcLJet_sgR);
      instance.SetDeleteArray(&deleteArray_vectorlEeventBuffercLcLJet_sgR);
      instance.SetDestructor(&destruct_vectorlEeventBuffercLcLJet_sgR);
      instance.AdoptCollectionProxyInfo(TCollectionProxyInfo::Generate(TCollectionProxyInfo::Pushback< vector<eventBuffer::Jet_s> >()));

      instance.AdoptAlternate(::ROOT::AddClassAlternate("vector<eventBuffer::Jet_s>","std::__1::vector<eventBuffer::Jet_s, std::__1::allocator<eventBuffer::Jet_s>>"));
      return &instance;
   }
   // Static variable to force the class initialization
   static ::ROOT::TGenericClassInfo *_R__UNIQUE_DICT_(Init) = GenerateInitInstanceLocal(static_cast<const vector<eventBuffer::Jet_s>*>(nullptr)); R__UseDummy(_R__UNIQUE_DICT_(Init));

   // Dictionary for non-ClassDef classes
   static TClass *vectorlEeventBuffercLcLJet_sgR_Dictionary() {
      TClass* theClass =::ROOT::GenerateInitInstanceLocal(static_cast<const vector<eventBuffer::Jet_s>*>(nullptr))->GetClass();
      vectorlEeventBuffercLcLJet_sgR_TClassManip(theClass);
   return theClass;
   }

   static void vectorlEeventBuffercLcLJet_sgR_TClassManip(TClass* ){
   }

} // end of namespace ROOT

namespace ROOT {
   // Wrappers around operator new
   static void *new_vectorlEeventBuffercLcLJet_sgR(void *p) {
      return  p ? ::new(static_cast<::ROOT::Internal::TOperatorNewHelper*>(p)) vector<eventBuffer::Jet_s> : new vector<eventBuffer::Jet_s>;
   }
   static void *newArray_vectorlEeventBuffercLcLJet_sgR(Long_t nElements, void *p) {
      return p ? ::new(static_cast<::ROOT::Internal::TOperatorNewHelper*>(p)) vector<eventBuffer::Jet_s>[nElements] : new vector<eventBuffer::Jet_s>[nElements];
   }
   // Wrapper around operator delete
   static void delete_vectorlEeventBuffercLcLJet_sgR(void *p) {
      delete (static_cast<vector<eventBuffer::Jet_s>*>(p));
   }
   static void deleteArray_vectorlEeventBuffercLcLJet_sgR(void *p) {
      delete [] (static_cast<vector<eventBuffer::Jet_s>*>(p));
   }
   static void destruct_vectorlEeventBuffercLcLJet_sgR(void *p) {
      typedef vector<eventBuffer::Jet_s> current_t;
      (static_cast<current_t*>(p))->~current_t();
   }
} // end of namespace ROOT for class vector<eventBuffer::Jet_s>

namespace ROOT {
   static TClass *vectorlEeventBuffercLcLIsoTrack_sgR_Dictionary();
   static void vectorlEeventBuffercLcLIsoTrack_sgR_TClassManip(TClass*);
   static void *new_vectorlEeventBuffercLcLIsoTrack_sgR(void *p = nullptr);
   static void *newArray_vectorlEeventBuffercLcLIsoTrack_sgR(Long_t size, void *p);
   static void delete_vectorlEeventBuffercLcLIsoTrack_sgR(void *p);
   static void deleteArray_vectorlEeventBuffercLcLIsoTrack_sgR(void *p);
   static void destruct_vectorlEeventBuffercLcLIsoTrack_sgR(void *p);

   // Function generating the singleton type initializer
   static TGenericClassInfo *GenerateInitInstanceLocal(const vector<eventBuffer::IsoTrack_s>*)
   {
      vector<eventBuffer::IsoTrack_s> *ptr = nullptr;
      static ::TVirtualIsAProxy* isa_proxy = new ::TIsAProxy(typeid(vector<eventBuffer::IsoTrack_s>));
      static ::ROOT::TGenericClassInfo 
         instance("vector<eventBuffer::IsoTrack_s>", -2, "vector", 389,
                  typeid(vector<eventBuffer::IsoTrack_s>), ::ROOT::Internal::DefineBehavior(ptr, ptr),
                  &vectorlEeventBuffercLcLIsoTrack_sgR_Dictionary, isa_proxy, 4,
                  sizeof(vector<eventBuffer::IsoTrack_s>) );
      instance.SetNew(&new_vectorlEeventBuffercLcLIsoTrack_sgR);
      instance.SetNewArray(&newArray_vectorlEeventBuffercLcLIsoTrack_sgR);
      instance.SetDelete(&delete_vectorlEeventBuffercLcLIsoTrack_sgR);
      instance.SetDeleteArray(&deleteArray_vectorlEeventBuffercLcLIsoTrack_sgR);
      instance.SetDestructor(&destruct_vectorlEeventBuffercLcLIsoTrack_sgR);
      instance.AdoptCollectionProxyInfo(TCollectionProxyInfo::Generate(TCollectionProxyInfo::Pushback< vector<eventBuffer::IsoTrack_s> >()));

      instance.AdoptAlternate(::ROOT::AddClassAlternate("vector<eventBuffer::IsoTrack_s>","std::__1::vector<eventBuffer::IsoTrack_s, std::__1::allocator<eventBuffer::IsoTrack_s>>"));
      return &instance;
   }
   // Static variable to force the class initialization
   static ::ROOT::TGenericClassInfo *_R__UNIQUE_DICT_(Init) = GenerateInitInstanceLocal(static_cast<const vector<eventBuffer::IsoTrack_s>*>(nullptr)); R__UseDummy(_R__UNIQUE_DICT_(Init));

   // Dictionary for non-ClassDef classes
   static TClass *vectorlEeventBuffercLcLIsoTrack_sgR_Dictionary() {
      TClass* theClass =::ROOT::GenerateInitInstanceLocal(static_cast<const vector<eventBuffer::IsoTrack_s>*>(nullptr))->GetClass();
      vectorlEeventBuffercLcLIsoTrack_sgR_TClassManip(theClass);
   return theClass;
   }

   static void vectorlEeventBuffercLcLIsoTrack_sgR_TClassManip(TClass* ){
   }

} // end of namespace ROOT

namespace ROOT {
   // Wrappers around operator new
   static void *new_vectorlEeventBuffercLcLIsoTrack_sgR(void *p) {
      return  p ? ::new(static_cast<::ROOT::Internal::TOperatorNewHelper*>(p)) vector<eventBuffer::IsoTrack_s> : new vector<eventBuffer::IsoTrack_s>;
   }
   static void *newArray_vectorlEeventBuffercLcLIsoTrack_sgR(Long_t nElements, void *p) {
      return p ? ::new(static_cast<::ROOT::Internal::TOperatorNewHelper*>(p)) vector<eventBuffer::IsoTrack_s>[nElements] : new vector<eventBuffer::IsoTrack_s>[nElements];
   }
   // Wrapper around operator delete
   static void delete_vectorlEeventBuffercLcLIsoTrack_sgR(void *p) {
      delete (static_cast<vector<eventBuffer::IsoTrack_s>*>(p));
   }
   static void deleteArray_vectorlEeventBuffercLcLIsoTrack_sgR(void *p) {
      delete [] (static_cast<vector<eventBuffer::IsoTrack_s>*>(p));
   }
   static void destruct_vectorlEeventBuffercLcLIsoTrack_sgR(void *p) {
      typedef vector<eventBuffer::IsoTrack_s> current_t;
      (static_cast<current_t*>(p))->~current_t();
   }
} // end of namespace ROOT for class vector<eventBuffer::IsoTrack_s>

namespace ROOT {
   static TClass *vectorlEeventBuffercLcLGenPart_sgR_Dictionary();
   static void vectorlEeventBuffercLcLGenPart_sgR_TClassManip(TClass*);
   static void *new_vectorlEeventBuffercLcLGenPart_sgR(void *p = nullptr);
   static void *newArray_vectorlEeventBuffercLcLGenPart_sgR(Long_t size, void *p);
   static void delete_vectorlEeventBuffercLcLGenPart_sgR(void *p);
   static void deleteArray_vectorlEeventBuffercLcLGenPart_sgR(void *p);
   static void destruct_vectorlEeventBuffercLcLGenPart_sgR(void *p);

   // Function generating the singleton type initializer
   static TGenericClassInfo *GenerateInitInstanceLocal(const vector<eventBuffer::GenPart_s>*)
   {
      vector<eventBuffer::GenPart_s> *ptr = nullptr;
      static ::TVirtualIsAProxy* isa_proxy = new ::TIsAProxy(typeid(vector<eventBuffer::GenPart_s>));
      static ::ROOT::TGenericClassInfo 
         instance("vector<eventBuffer::GenPart_s>", -2, "vector", 389,
                  typeid(vector<eventBuffer::GenPart_s>), ::ROOT::Internal::DefineBehavior(ptr, ptr),
                  &vectorlEeventBuffercLcLGenPart_sgR_Dictionary, isa_proxy, 4,
                  sizeof(vector<eventBuffer::GenPart_s>) );
      instance.SetNew(&new_vectorlEeventBuffercLcLGenPart_sgR);
      instance.SetNewArray(&newArray_vectorlEeventBuffercLcLGenPart_sgR);
      instance.SetDelete(&delete_vectorlEeventBuffercLcLGenPart_sgR);
      instance.SetDeleteArray(&deleteArray_vectorlEeventBuffercLcLGenPart_sgR);
      instance.SetDestructor(&destruct_vectorlEeventBuffercLcLGenPart_sgR);
      instance.AdoptCollectionProxyInfo(TCollectionProxyInfo::Generate(TCollectionProxyInfo::Pushback< vector<eventBuffer::GenPart_s> >()));

      instance.AdoptAlternate(::ROOT::AddClassAlternate("vector<eventBuffer::GenPart_s>","std::__1::vector<eventBuffer::GenPart_s, std::__1::allocator<eventBuffer::GenPart_s>>"));
      return &instance;
   }
   // Static variable to force the class initialization
   static ::ROOT::TGenericClassInfo *_R__UNIQUE_DICT_(Init) = GenerateInitInstanceLocal(static_cast<const vector<eventBuffer::GenPart_s>*>(nullptr)); R__UseDummy(_R__UNIQUE_DICT_(Init));

   // Dictionary for non-ClassDef classes
   static TClass *vectorlEeventBuffercLcLGenPart_sgR_Dictionary() {
      TClass* theClass =::ROOT::GenerateInitInstanceLocal(static_cast<const vector<eventBuffer::GenPart_s>*>(nullptr))->GetClass();
      vectorlEeventBuffercLcLGenPart_sgR_TClassManip(theClass);
   return theClass;
   }

   static void vectorlEeventBuffercLcLGenPart_sgR_TClassManip(TClass* ){
   }

} // end of namespace ROOT

namespace ROOT {
   // Wrappers around operator new
   static void *new_vectorlEeventBuffercLcLGenPart_sgR(void *p) {
      return  p ? ::new(static_cast<::ROOT::Internal::TOperatorNewHelper*>(p)) vector<eventBuffer::GenPart_s> : new vector<eventBuffer::GenPart_s>;
   }
   static void *newArray_vectorlEeventBuffercLcLGenPart_sgR(Long_t nElements, void *p) {
      return p ? ::new(static_cast<::ROOT::Internal::TOperatorNewHelper*>(p)) vector<eventBuffer::GenPart_s>[nElements] : new vector<eventBuffer::GenPart_s>[nElements];
   }
   // Wrapper around operator delete
   static void delete_vectorlEeventBuffercLcLGenPart_sgR(void *p) {
      delete (static_cast<vector<eventBuffer::GenPart_s>*>(p));
   }
   static void deleteArray_vectorlEeventBuffercLcLGenPart_sgR(void *p) {
      delete [] (static_cast<vector<eventBuffer::GenPart_s>*>(p));
   }
   static void destruct_vectorlEeventBuffercLcLGenPart_sgR(void *p) {
      typedef vector<eventBuffer::GenPart_s> current_t;
      (static_cast<current_t*>(p))->~current_t();
   }
} // end of namespace ROOT for class vector<eventBuffer::GenPart_s>

namespace ROOT {
   static TClass *vectorlEeventBuffercLcLGenJet_sgR_Dictionary();
   static void vectorlEeventBuffercLcLGenJet_sgR_TClassManip(TClass*);
   static void *new_vectorlEeventBuffercLcLGenJet_sgR(void *p = nullptr);
   static void *newArray_vectorlEeventBuffercLcLGenJet_sgR(Long_t size, void *p);
   static void delete_vectorlEeventBuffercLcLGenJet_sgR(void *p);
   static void deleteArray_vectorlEeventBuffercLcLGenJet_sgR(void *p);
   static void destruct_vectorlEeventBuffercLcLGenJet_sgR(void *p);

   // Function generating the singleton type initializer
   static TGenericClassInfo *GenerateInitInstanceLocal(const vector<eventBuffer::GenJet_s>*)
   {
      vector<eventBuffer::GenJet_s> *ptr = nullptr;
      static ::TVirtualIsAProxy* isa_proxy = new ::TIsAProxy(typeid(vector<eventBuffer::GenJet_s>));
      static ::ROOT::TGenericClassInfo 
         instance("vector<eventBuffer::GenJet_s>", -2, "vector", 389,
                  typeid(vector<eventBuffer::GenJet_s>), ::ROOT::Internal::DefineBehavior(ptr, ptr),
                  &vectorlEeventBuffercLcLGenJet_sgR_Dictionary, isa_proxy, 4,
                  sizeof(vector<eventBuffer::GenJet_s>) );
      instance.SetNew(&new_vectorlEeventBuffercLcLGenJet_sgR);
      instance.SetNewArray(&newArray_vectorlEeventBuffercLcLGenJet_sgR);
      instance.SetDelete(&delete_vectorlEeventBuffercLcLGenJet_sgR);
      instance.SetDeleteArray(&deleteArray_vectorlEeventBuffercLcLGenJet_sgR);
      instance.SetDestructor(&destruct_vectorlEeventBuffercLcLGenJet_sgR);
      instance.AdoptCollectionProxyInfo(TCollectionProxyInfo::Generate(TCollectionProxyInfo::Pushback< vector<eventBuffer::GenJet_s> >()));

      instance.AdoptAlternate(::ROOT::AddClassAlternate("vector<eventBuffer::GenJet_s>","std::__1::vector<eventBuffer::GenJet_s, std::__1::allocator<eventBuffer::GenJet_s>>"));
      return &instance;
   }
   // Static variable to force the class initialization
   static ::ROOT::TGenericClassInfo *_R__UNIQUE_DICT_(Init) = GenerateInitInstanceLocal(static_cast<const vector<eventBuffer::GenJet_s>*>(nullptr)); R__UseDummy(_R__UNIQUE_DICT_(Init));

   // Dictionary for non-ClassDef classes
   static TClass *vectorlEeventBuffercLcLGenJet_sgR_Dictionary() {
      TClass* theClass =::ROOT::GenerateInitInstanceLocal(static_cast<const vector<eventBuffer::GenJet_s>*>(nullptr))->GetClass();
      vectorlEeventBuffercLcLGenJet_sgR_TClassManip(theClass);
   return theClass;
   }

   static void vectorlEeventBuffercLcLGenJet_sgR_TClassManip(TClass* ){
   }

} // end of namespace ROOT

namespace ROOT {
   // Wrappers around operator new
   static void *new_vectorlEeventBuffercLcLGenJet_sgR(void *p) {
      return  p ? ::new(static_cast<::ROOT::Internal::TOperatorNewHelper*>(p)) vector<eventBuffer::GenJet_s> : new vector<eventBuffer::GenJet_s>;
   }
   static void *newArray_vectorlEeventBuffercLcLGenJet_sgR(Long_t nElements, void *p) {
      return p ? ::new(static_cast<::ROOT::Internal::TOperatorNewHelper*>(p)) vector<eventBuffer::GenJet_s>[nElements] : new vector<eventBuffer::GenJet_s>[nElements];
   }
   // Wrapper around operator delete
   static void delete_vectorlEeventBuffercLcLGenJet_sgR(void *p) {
      delete (static_cast<vector<eventBuffer::GenJet_s>*>(p));
   }
   static void deleteArray_vectorlEeventBuffercLcLGenJet_sgR(void *p) {
      delete [] (static_cast<vector<eventBuffer::GenJet_s>*>(p));
   }
   static void destruct_vectorlEeventBuffercLcLGenJet_sgR(void *p) {
      typedef vector<eventBuffer::GenJet_s> current_t;
      (static_cast<current_t*>(p))->~current_t();
   }
} // end of namespace ROOT for class vector<eventBuffer::GenJet_s>

namespace ROOT {
   static TClass *vectorlEeventBuffercLcLGenJetAK8_sgR_Dictionary();
   static void vectorlEeventBuffercLcLGenJetAK8_sgR_TClassManip(TClass*);
   static void *new_vectorlEeventBuffercLcLGenJetAK8_sgR(void *p = nullptr);
   static void *newArray_vectorlEeventBuffercLcLGenJetAK8_sgR(Long_t size, void *p);
   static void delete_vectorlEeventBuffercLcLGenJetAK8_sgR(void *p);
   static void deleteArray_vectorlEeventBuffercLcLGenJetAK8_sgR(void *p);
   static void destruct_vectorlEeventBuffercLcLGenJetAK8_sgR(void *p);

   // Function generating the singleton type initializer
   static TGenericClassInfo *GenerateInitInstanceLocal(const vector<eventBuffer::GenJetAK8_s>*)
   {
      vector<eventBuffer::GenJetAK8_s> *ptr = nullptr;
      static ::TVirtualIsAProxy* isa_proxy = new ::TIsAProxy(typeid(vector<eventBuffer::GenJetAK8_s>));
      static ::ROOT::TGenericClassInfo 
         instance("vector<eventBuffer::GenJetAK8_s>", -2, "vector", 389,
                  typeid(vector<eventBuffer::GenJetAK8_s>), ::ROOT::Internal::DefineBehavior(ptr, ptr),
                  &vectorlEeventBuffercLcLGenJetAK8_sgR_Dictionary, isa_proxy, 4,
                  sizeof(vector<eventBuffer::GenJetAK8_s>) );
      instance.SetNew(&new_vectorlEeventBuffercLcLGenJetAK8_sgR);
      instance.SetNewArray(&newArray_vectorlEeventBuffercLcLGenJetAK8_sgR);
      instance.SetDelete(&delete_vectorlEeventBuffercLcLGenJetAK8_sgR);
      instance.SetDeleteArray(&deleteArray_vectorlEeventBuffercLcLGenJetAK8_sgR);
      instance.SetDestructor(&destruct_vectorlEeventBuffercLcLGenJetAK8_sgR);
      instance.AdoptCollectionProxyInfo(TCollectionProxyInfo::Generate(TCollectionProxyInfo::Pushback< vector<eventBuffer::GenJetAK8_s> >()));

      instance.AdoptAlternate(::ROOT::AddClassAlternate("vector<eventBuffer::GenJetAK8_s>","std::__1::vector<eventBuffer::GenJetAK8_s, std::__1::allocator<eventBuffer::GenJetAK8_s>>"));
      return &instance;
   }
   // Static variable to force the class initialization
   static ::ROOT::TGenericClassInfo *_R__UNIQUE_DICT_(Init) = GenerateInitInstanceLocal(static_cast<const vector<eventBuffer::GenJetAK8_s>*>(nullptr)); R__UseDummy(_R__UNIQUE_DICT_(Init));

   // Dictionary for non-ClassDef classes
   static TClass *vectorlEeventBuffercLcLGenJetAK8_sgR_Dictionary() {
      TClass* theClass =::ROOT::GenerateInitInstanceLocal(static_cast<const vector<eventBuffer::GenJetAK8_s>*>(nullptr))->GetClass();
      vectorlEeventBuffercLcLGenJetAK8_sgR_TClassManip(theClass);
   return theClass;
   }

   static void vectorlEeventBuffercLcLGenJetAK8_sgR_TClassManip(TClass* ){
   }

} // end of namespace ROOT

namespace ROOT {
   // Wrappers around operator new
   static void *new_vectorlEeventBuffercLcLGenJetAK8_sgR(void *p) {
      return  p ? ::new(static_cast<::ROOT::Internal::TOperatorNewHelper*>(p)) vector<eventBuffer::GenJetAK8_s> : new vector<eventBuffer::GenJetAK8_s>;
   }
   static void *newArray_vectorlEeventBuffercLcLGenJetAK8_sgR(Long_t nElements, void *p) {
      return p ? ::new(static_cast<::ROOT::Internal::TOperatorNewHelper*>(p)) vector<eventBuffer::GenJetAK8_s>[nElements] : new vector<eventBuffer::GenJetAK8_s>[nElements];
   }
   // Wrapper around operator delete
   static void delete_vectorlEeventBuffercLcLGenJetAK8_sgR(void *p) {
      delete (static_cast<vector<eventBuffer::GenJetAK8_s>*>(p));
   }
   static void deleteArray_vectorlEeventBuffercLcLGenJetAK8_sgR(void *p) {
      delete [] (static_cast<vector<eventBuffer::GenJetAK8_s>*>(p));
   }
   static void destruct_vectorlEeventBuffercLcLGenJetAK8_sgR(void *p) {
      typedef vector<eventBuffer::GenJetAK8_s> current_t;
      (static_cast<current_t*>(p))->~current_t();
   }
} // end of namespace ROOT for class vector<eventBuffer::GenJetAK8_s>

namespace ROOT {
   static TClass *vectorlEeventBuffercLcLGenIsolatedPhoton_sgR_Dictionary();
   static void vectorlEeventBuffercLcLGenIsolatedPhoton_sgR_TClassManip(TClass*);
   static void *new_vectorlEeventBuffercLcLGenIsolatedPhoton_sgR(void *p = nullptr);
   static void *newArray_vectorlEeventBuffercLcLGenIsolatedPhoton_sgR(Long_t size, void *p);
   static void delete_vectorlEeventBuffercLcLGenIsolatedPhoton_sgR(void *p);
   static void deleteArray_vectorlEeventBuffercLcLGenIsolatedPhoton_sgR(void *p);
   static void destruct_vectorlEeventBuffercLcLGenIsolatedPhoton_sgR(void *p);

   // Function generating the singleton type initializer
   static TGenericClassInfo *GenerateInitInstanceLocal(const vector<eventBuffer::GenIsolatedPhoton_s>*)
   {
      vector<eventBuffer::GenIsolatedPhoton_s> *ptr = nullptr;
      static ::TVirtualIsAProxy* isa_proxy = new ::TIsAProxy(typeid(vector<eventBuffer::GenIsolatedPhoton_s>));
      static ::ROOT::TGenericClassInfo 
         instance("vector<eventBuffer::GenIsolatedPhoton_s>", -2, "vector", 389,
                  typeid(vector<eventBuffer::GenIsolatedPhoton_s>), ::ROOT::Internal::DefineBehavior(ptr, ptr),
                  &vectorlEeventBuffercLcLGenIsolatedPhoton_sgR_Dictionary, isa_proxy, 4,
                  sizeof(vector<eventBuffer::GenIsolatedPhoton_s>) );
      instance.SetNew(&new_vectorlEeventBuffercLcLGenIsolatedPhoton_sgR);
      instance.SetNewArray(&newArray_vectorlEeventBuffercLcLGenIsolatedPhoton_sgR);
      instance.SetDelete(&delete_vectorlEeventBuffercLcLGenIsolatedPhoton_sgR);
      instance.SetDeleteArray(&deleteArray_vectorlEeventBuffercLcLGenIsolatedPhoton_sgR);
      instance.SetDestructor(&destruct_vectorlEeventBuffercLcLGenIsolatedPhoton_sgR);
      instance.AdoptCollectionProxyInfo(TCollectionProxyInfo::Generate(TCollectionProxyInfo::Pushback< vector<eventBuffer::GenIsolatedPhoton_s> >()));

      instance.AdoptAlternate(::ROOT::AddClassAlternate("vector<eventBuffer::GenIsolatedPhoton_s>","std::__1::vector<eventBuffer::GenIsolatedPhoton_s, std::__1::allocator<eventBuffer::GenIsolatedPhoton_s>>"));
      return &instance;
   }
   // Static variable to force the class initialization
   static ::ROOT::TGenericClassInfo *_R__UNIQUE_DICT_(Init) = GenerateInitInstanceLocal(static_cast<const vector<eventBuffer::GenIsolatedPhoton_s>*>(nullptr)); R__UseDummy(_R__UNIQUE_DICT_(Init));

   // Dictionary for non-ClassDef classes
   static TClass *vectorlEeventBuffercLcLGenIsolatedPhoton_sgR_Dictionary() {
      TClass* theClass =::ROOT::GenerateInitInstanceLocal(static_cast<const vector<eventBuffer::GenIsolatedPhoton_s>*>(nullptr))->GetClass();
      vectorlEeventBuffercLcLGenIsolatedPhoton_sgR_TClassManip(theClass);
   return theClass;
   }

   static void vectorlEeventBuffercLcLGenIsolatedPhoton_sgR_TClassManip(TClass* ){
   }

} // end of namespace ROOT

namespace ROOT {
   // Wrappers around operator new
   static void *new_vectorlEeventBuffercLcLGenIsolatedPhoton_sgR(void *p) {
      return  p ? ::new(static_cast<::ROOT::Internal::TOperatorNewHelper*>(p)) vector<eventBuffer::GenIsolatedPhoton_s> : new vector<eventBuffer::GenIsolatedPhoton_s>;
   }
   static void *newArray_vectorlEeventBuffercLcLGenIsolatedPhoton_sgR(Long_t nElements, void *p) {
      return p ? ::new(static_cast<::ROOT::Internal::TOperatorNewHelper*>(p)) vector<eventBuffer::GenIsolatedPhoton_s>[nElements] : new vector<eventBuffer::GenIsolatedPhoton_s>[nElements];
   }
   // Wrapper around operator delete
   static void delete_vectorlEeventBuffercLcLGenIsolatedPhoton_sgR(void *p) {
      delete (static_cast<vector<eventBuffer::GenIsolatedPhoton_s>*>(p));
   }
   static void deleteArray_vectorlEeventBuffercLcLGenIsolatedPhoton_sgR(void *p) {
      delete [] (static_cast<vector<eventBuffer::GenIsolatedPhoton_s>*>(p));
   }
   static void destruct_vectorlEeventBuffercLcLGenIsolatedPhoton_sgR(void *p) {
      typedef vector<eventBuffer::GenIsolatedPhoton_s> current_t;
      (static_cast<current_t*>(p))->~current_t();
   }
} // end of namespace ROOT for class vector<eventBuffer::GenIsolatedPhoton_s>

namespace ROOT {
   static TClass *vectorlEeventBuffercLcLFsrPhoton_sgR_Dictionary();
   static void vectorlEeventBuffercLcLFsrPhoton_sgR_TClassManip(TClass*);
   static void *new_vectorlEeventBuffercLcLFsrPhoton_sgR(void *p = nullptr);
   static void *newArray_vectorlEeventBuffercLcLFsrPhoton_sgR(Long_t size, void *p);
   static void delete_vectorlEeventBuffercLcLFsrPhoton_sgR(void *p);
   static void deleteArray_vectorlEeventBuffercLcLFsrPhoton_sgR(void *p);
   static void destruct_vectorlEeventBuffercLcLFsrPhoton_sgR(void *p);

   // Function generating the singleton type initializer
   static TGenericClassInfo *GenerateInitInstanceLocal(const vector<eventBuffer::FsrPhoton_s>*)
   {
      vector<eventBuffer::FsrPhoton_s> *ptr = nullptr;
      static ::TVirtualIsAProxy* isa_proxy = new ::TIsAProxy(typeid(vector<eventBuffer::FsrPhoton_s>));
      static ::ROOT::TGenericClassInfo 
         instance("vector<eventBuffer::FsrPhoton_s>", -2, "vector", 389,
                  typeid(vector<eventBuffer::FsrPhoton_s>), ::ROOT::Internal::DefineBehavior(ptr, ptr),
                  &vectorlEeventBuffercLcLFsrPhoton_sgR_Dictionary, isa_proxy, 4,
                  sizeof(vector<eventBuffer::FsrPhoton_s>) );
      instance.SetNew(&new_vectorlEeventBuffercLcLFsrPhoton_sgR);
      instance.SetNewArray(&newArray_vectorlEeventBuffercLcLFsrPhoton_sgR);
      instance.SetDelete(&delete_vectorlEeventBuffercLcLFsrPhoton_sgR);
      instance.SetDeleteArray(&deleteArray_vectorlEeventBuffercLcLFsrPhoton_sgR);
      instance.SetDestructor(&destruct_vectorlEeventBuffercLcLFsrPhoton_sgR);
      instance.AdoptCollectionProxyInfo(TCollectionProxyInfo::Generate(TCollectionProxyInfo::Pushback< vector<eventBuffer::FsrPhoton_s> >()));

      instance.AdoptAlternate(::ROOT::AddClassAlternate("vector<eventBuffer::FsrPhoton_s>","std::__1::vector<eventBuffer::FsrPhoton_s, std::__1::allocator<eventBuffer::FsrPhoton_s>>"));
      return &instance;
   }
   // Static variable to force the class initialization
   static ::ROOT::TGenericClassInfo *_R__UNIQUE_DICT_(Init) = GenerateInitInstanceLocal(static_cast<const vector<eventBuffer::FsrPhoton_s>*>(nullptr)); R__UseDummy(_R__UNIQUE_DICT_(Init));

   // Dictionary for non-ClassDef classes
   static TClass *vectorlEeventBuffercLcLFsrPhoton_sgR_Dictionary() {
      TClass* theClass =::ROOT::GenerateInitInstanceLocal(static_cast<const vector<eventBuffer::FsrPhoton_s>*>(nullptr))->GetClass();
      vectorlEeventBuffercLcLFsrPhoton_sgR_TClassManip(theClass);
   return theClass;
   }

   static void vectorlEeventBuffercLcLFsrPhoton_sgR_TClassManip(TClass* ){
   }

} // end of namespace ROOT

namespace ROOT {
   // Wrappers around operator new
   static void *new_vectorlEeventBuffercLcLFsrPhoton_sgR(void *p) {
      return  p ? ::new(static_cast<::ROOT::Internal::TOperatorNewHelper*>(p)) vector<eventBuffer::FsrPhoton_s> : new vector<eventBuffer::FsrPhoton_s>;
   }
   static void *newArray_vectorlEeventBuffercLcLFsrPhoton_sgR(Long_t nElements, void *p) {
      return p ? ::new(static_cast<::ROOT::Internal::TOperatorNewHelper*>(p)) vector<eventBuffer::FsrPhoton_s>[nElements] : new vector<eventBuffer::FsrPhoton_s>[nElements];
   }
   // Wrapper around operator delete
   static void delete_vectorlEeventBuffercLcLFsrPhoton_sgR(void *p) {
      delete (static_cast<vector<eventBuffer::FsrPhoton_s>*>(p));
   }
   static void deleteArray_vectorlEeventBuffercLcLFsrPhoton_sgR(void *p) {
      delete [] (static_cast<vector<eventBuffer::FsrPhoton_s>*>(p));
   }
   static void destruct_vectorlEeventBuffercLcLFsrPhoton_sgR(void *p) {
      typedef vector<eventBuffer::FsrPhoton_s> current_t;
      (static_cast<current_t*>(p))->~current_t();
   }
} // end of namespace ROOT for class vector<eventBuffer::FsrPhoton_s>

namespace ROOT {
   static TClass *vectorlEeventBuffercLcLFatJet_sgR_Dictionary();
   static void vectorlEeventBuffercLcLFatJet_sgR_TClassManip(TClass*);
   static void *new_vectorlEeventBuffercLcLFatJet_sgR(void *p = nullptr);
   static void *newArray_vectorlEeventBuffercLcLFatJet_sgR(Long_t size, void *p);
   static void delete_vectorlEeventBuffercLcLFatJet_sgR(void *p);
   static void deleteArray_vectorlEeventBuffercLcLFatJet_sgR(void *p);
   static void destruct_vectorlEeventBuffercLcLFatJet_sgR(void *p);

   // Function generating the singleton type initializer
   static TGenericClassInfo *GenerateInitInstanceLocal(const vector<eventBuffer::FatJet_s>*)
   {
      vector<eventBuffer::FatJet_s> *ptr = nullptr;
      static ::TVirtualIsAProxy* isa_proxy = new ::TIsAProxy(typeid(vector<eventBuffer::FatJet_s>));
      static ::ROOT::TGenericClassInfo 
         instance("vector<eventBuffer::FatJet_s>", -2, "vector", 389,
                  typeid(vector<eventBuffer::FatJet_s>), ::ROOT::Internal::DefineBehavior(ptr, ptr),
                  &vectorlEeventBuffercLcLFatJet_sgR_Dictionary, isa_proxy, 4,
                  sizeof(vector<eventBuffer::FatJet_s>) );
      instance.SetNew(&new_vectorlEeventBuffercLcLFatJet_sgR);
      instance.SetNewArray(&newArray_vectorlEeventBuffercLcLFatJet_sgR);
      instance.SetDelete(&delete_vectorlEeventBuffercLcLFatJet_sgR);
      instance.SetDeleteArray(&deleteArray_vectorlEeventBuffercLcLFatJet_sgR);
      instance.SetDestructor(&destruct_vectorlEeventBuffercLcLFatJet_sgR);
      instance.AdoptCollectionProxyInfo(TCollectionProxyInfo::Generate(TCollectionProxyInfo::Pushback< vector<eventBuffer::FatJet_s> >()));

      instance.AdoptAlternate(::ROOT::AddClassAlternate("vector<eventBuffer::FatJet_s>","std::__1::vector<eventBuffer::FatJet_s, std::__1::allocator<eventBuffer::FatJet_s>>"));
      return &instance;
   }
   // Static variable to force the class initialization
   static ::ROOT::TGenericClassInfo *_R__UNIQUE_DICT_(Init) = GenerateInitInstanceLocal(static_cast<const vector<eventBuffer::FatJet_s>*>(nullptr)); R__UseDummy(_R__UNIQUE_DICT_(Init));

   // Dictionary for non-ClassDef classes
   static TClass *vectorlEeventBuffercLcLFatJet_sgR_Dictionary() {
      TClass* theClass =::ROOT::GenerateInitInstanceLocal(static_cast<const vector<eventBuffer::FatJet_s>*>(nullptr))->GetClass();
      vectorlEeventBuffercLcLFatJet_sgR_TClassManip(theClass);
   return theClass;
   }

   static void vectorlEeventBuffercLcLFatJet_sgR_TClassManip(TClass* ){
   }

} // end of namespace ROOT

namespace ROOT {
   // Wrappers around operator new
   static void *new_vectorlEeventBuffercLcLFatJet_sgR(void *p) {
      return  p ? ::new(static_cast<::ROOT::Internal::TOperatorNewHelper*>(p)) vector<eventBuffer::FatJet_s> : new vector<eventBuffer::FatJet_s>;
   }
   static void *newArray_vectorlEeventBuffercLcLFatJet_sgR(Long_t nElements, void *p) {
      return p ? ::new(static_cast<::ROOT::Internal::TOperatorNewHelper*>(p)) vector<eventBuffer::FatJet_s>[nElements] : new vector<eventBuffer::FatJet_s>[nElements];
   }
   // Wrapper around operator delete
   static void delete_vectorlEeventBuffercLcLFatJet_sgR(void *p) {
      delete (static_cast<vector<eventBuffer::FatJet_s>*>(p));
   }
   static void deleteArray_vectorlEeventBuffercLcLFatJet_sgR(void *p) {
      delete [] (static_cast<vector<eventBuffer::FatJet_s>*>(p));
   }
   static void destruct_vectorlEeventBuffercLcLFatJet_sgR(void *p) {
      typedef vector<eventBuffer::FatJet_s> current_t;
      (static_cast<current_t*>(p))->~current_t();
   }
} // end of namespace ROOT for class vector<eventBuffer::FatJet_s>

namespace ROOT {
   static TClass *vectorlEeventBuffercLcLElectron_sgR_Dictionary();
   static void vectorlEeventBuffercLcLElectron_sgR_TClassManip(TClass*);
   static void *new_vectorlEeventBuffercLcLElectron_sgR(void *p = nullptr);
   static void *newArray_vectorlEeventBuffercLcLElectron_sgR(Long_t size, void *p);
   static void delete_vectorlEeventBuffercLcLElectron_sgR(void *p);
   static void deleteArray_vectorlEeventBuffercLcLElectron_sgR(void *p);
   static void destruct_vectorlEeventBuffercLcLElectron_sgR(void *p);

   // Function generating the singleton type initializer
   static TGenericClassInfo *GenerateInitInstanceLocal(const vector<eventBuffer::Electron_s>*)
   {
      vector<eventBuffer::Electron_s> *ptr = nullptr;
      static ::TVirtualIsAProxy* isa_proxy = new ::TIsAProxy(typeid(vector<eventBuffer::Electron_s>));
      static ::ROOT::TGenericClassInfo 
         instance("vector<eventBuffer::Electron_s>", -2, "vector", 389,
                  typeid(vector<eventBuffer::Electron_s>), ::ROOT::Internal::DefineBehavior(ptr, ptr),
                  &vectorlEeventBuffercLcLElectron_sgR_Dictionary, isa_proxy, 4,
                  sizeof(vector<eventBuffer::Electron_s>) );
      instance.SetNew(&new_vectorlEeventBuffercLcLElectron_sgR);
      instance.SetNewArray(&newArray_vectorlEeventBuffercLcLElectron_sgR);
      instance.SetDelete(&delete_vectorlEeventBuffercLcLElectron_sgR);
      instance.SetDeleteArray(&deleteArray_vectorlEeventBuffercLcLElectron_sgR);
      instance.SetDestructor(&destruct_vectorlEeventBuffercLcLElectron_sgR);
      instance.AdoptCollectionProxyInfo(TCollectionProxyInfo::Generate(TCollectionProxyInfo::Pushback< vector<eventBuffer::Electron_s> >()));

      instance.AdoptAlternate(::ROOT::AddClassAlternate("vector<eventBuffer::Electron_s>","std::__1::vector<eventBuffer::Electron_s, std::__1::allocator<eventBuffer::Electron_s>>"));
      return &instance;
   }
   // Static variable to force the class initialization
   static ::ROOT::TGenericClassInfo *_R__UNIQUE_DICT_(Init) = GenerateInitInstanceLocal(static_cast<const vector<eventBuffer::Electron_s>*>(nullptr)); R__UseDummy(_R__UNIQUE_DICT_(Init));

   // Dictionary for non-ClassDef classes
   static TClass *vectorlEeventBuffercLcLElectron_sgR_Dictionary() {
      TClass* theClass =::ROOT::GenerateInitInstanceLocal(static_cast<const vector<eventBuffer::Electron_s>*>(nullptr))->GetClass();
      vectorlEeventBuffercLcLElectron_sgR_TClassManip(theClass);
   return theClass;
   }

   static void vectorlEeventBuffercLcLElectron_sgR_TClassManip(TClass* ){
   }

} // end of namespace ROOT

namespace ROOT {
   // Wrappers around operator new
   static void *new_vectorlEeventBuffercLcLElectron_sgR(void *p) {
      return  p ? ::new(static_cast<::ROOT::Internal::TOperatorNewHelper*>(p)) vector<eventBuffer::Electron_s> : new vector<eventBuffer::Electron_s>;
   }
   static void *newArray_vectorlEeventBuffercLcLElectron_sgR(Long_t nElements, void *p) {
      return p ? ::new(static_cast<::ROOT::Internal::TOperatorNewHelper*>(p)) vector<eventBuffer::Electron_s>[nElements] : new vector<eventBuffer::Electron_s>[nElements];
   }
   // Wrapper around operator delete
   static void delete_vectorlEeventBuffercLcLElectron_sgR(void *p) {
      delete (static_cast<vector<eventBuffer::Electron_s>*>(p));
   }
   static void deleteArray_vectorlEeventBuffercLcLElectron_sgR(void *p) {
      delete [] (static_cast<vector<eventBuffer::Electron_s>*>(p));
   }
   static void destruct_vectorlEeventBuffercLcLElectron_sgR(void *p) {
      typedef vector<eventBuffer::Electron_s> current_t;
      (static_cast<current_t*>(p))->~current_t();
   }
} // end of namespace ROOT for class vector<eventBuffer::Electron_s>

namespace ROOT {
   static TClass *vectorlEeventBuffercLcLCorrT1METJet_sgR_Dictionary();
   static void vectorlEeventBuffercLcLCorrT1METJet_sgR_TClassManip(TClass*);
   static void *new_vectorlEeventBuffercLcLCorrT1METJet_sgR(void *p = nullptr);
   static void *newArray_vectorlEeventBuffercLcLCorrT1METJet_sgR(Long_t size, void *p);
   static void delete_vectorlEeventBuffercLcLCorrT1METJet_sgR(void *p);
   static void deleteArray_vectorlEeventBuffercLcLCorrT1METJet_sgR(void *p);
   static void destruct_vectorlEeventBuffercLcLCorrT1METJet_sgR(void *p);

   // Function generating the singleton type initializer
   static TGenericClassInfo *GenerateInitInstanceLocal(const vector<eventBuffer::CorrT1METJet_s>*)
   {
      vector<eventBuffer::CorrT1METJet_s> *ptr = nullptr;
      static ::TVirtualIsAProxy* isa_proxy = new ::TIsAProxy(typeid(vector<eventBuffer::CorrT1METJet_s>));
      static ::ROOT::TGenericClassInfo 
         instance("vector<eventBuffer::CorrT1METJet_s>", -2, "vector", 389,
                  typeid(vector<eventBuffer::CorrT1METJet_s>), ::ROOT::Internal::DefineBehavior(ptr, ptr),
                  &vectorlEeventBuffercLcLCorrT1METJet_sgR_Dictionary, isa_proxy, 4,
                  sizeof(vector<eventBuffer::CorrT1METJet_s>) );
      instance.SetNew(&new_vectorlEeventBuffercLcLCorrT1METJet_sgR);
      instance.SetNewArray(&newArray_vectorlEeventBuffercLcLCorrT1METJet_sgR);
      instance.SetDelete(&delete_vectorlEeventBuffercLcLCorrT1METJet_sgR);
      instance.SetDeleteArray(&deleteArray_vectorlEeventBuffercLcLCorrT1METJet_sgR);
      instance.SetDestructor(&destruct_vectorlEeventBuffercLcLCorrT1METJet_sgR);
      instance.AdoptCollectionProxyInfo(TCollectionProxyInfo::Generate(TCollectionProxyInfo::Pushback< vector<eventBuffer::CorrT1METJet_s> >()));

      instance.AdoptAlternate(::ROOT::AddClassAlternate("vector<eventBuffer::CorrT1METJet_s>","std::__1::vector<eventBuffer::CorrT1METJet_s, std::__1::allocator<eventBuffer::CorrT1METJet_s>>"));
      return &instance;
   }
   // Static variable to force the class initialization
   static ::ROOT::TGenericClassInfo *_R__UNIQUE_DICT_(Init) = GenerateInitInstanceLocal(static_cast<const vector<eventBuffer::CorrT1METJet_s>*>(nullptr)); R__UseDummy(_R__UNIQUE_DICT_(Init));

   // Dictionary for non-ClassDef classes
   static TClass *vectorlEeventBuffercLcLCorrT1METJet_sgR_Dictionary() {
      TClass* theClass =::ROOT::GenerateInitInstanceLocal(static_cast<const vector<eventBuffer::CorrT1METJet_s>*>(nullptr))->GetClass();
      vectorlEeventBuffercLcLCorrT1METJet_sgR_TClassManip(theClass);
   return theClass;
   }

   static void vectorlEeventBuffercLcLCorrT1METJet_sgR_TClassManip(TClass* ){
   }

} // end of namespace ROOT

namespace ROOT {
   // Wrappers around operator new
   static void *new_vectorlEeventBuffercLcLCorrT1METJet_sgR(void *p) {
      return  p ? ::new(static_cast<::ROOT::Internal::TOperatorNewHelper*>(p)) vector<eventBuffer::CorrT1METJet_s> : new vector<eventBuffer::CorrT1METJet_s>;
   }
   static void *newArray_vectorlEeventBuffercLcLCorrT1METJet_sgR(Long_t nElements, void *p) {
      return p ? ::new(static_cast<::ROOT::Internal::TOperatorNewHelper*>(p)) vector<eventBuffer::CorrT1METJet_s>[nElements] : new vector<eventBuffer::CorrT1METJet_s>[nElements];
   }
   // Wrapper around operator delete
   static void delete_vectorlEeventBuffercLcLCorrT1METJet_sgR(void *p) {
      delete (static_cast<vector<eventBuffer::CorrT1METJet_s>*>(p));
   }
   static void deleteArray_vectorlEeventBuffercLcLCorrT1METJet_sgR(void *p) {
      delete [] (static_cast<vector<eventBuffer::CorrT1METJet_s>*>(p));
   }
   static void destruct_vectorlEeventBuffercLcLCorrT1METJet_sgR(void *p) {
      typedef vector<eventBuffer::CorrT1METJet_s> current_t;
      (static_cast<current_t*>(p))->~current_t();
   }
} // end of namespace ROOT for class vector<eventBuffer::CorrT1METJet_s>

namespace ROOT {
   static TClass *vectorlEdoublegR_Dictionary();
   static void vectorlEdoublegR_TClassManip(TClass*);
   static void *new_vectorlEdoublegR(void *p = nullptr);
   static void *newArray_vectorlEdoublegR(Long_t size, void *p);
   static void delete_vectorlEdoublegR(void *p);
   static void deleteArray_vectorlEdoublegR(void *p);
   static void destruct_vectorlEdoublegR(void *p);

   // Function generating the singleton type initializer
   static TGenericClassInfo *GenerateInitInstanceLocal(const vector<double>*)
   {
      vector<double> *ptr = nullptr;
      static ::TVirtualIsAProxy* isa_proxy = new ::TIsAProxy(typeid(vector<double>));
      static ::ROOT::TGenericClassInfo 
         instance("vector<double>", -2, "vector", 389,
                  typeid(vector<double>), ::ROOT::Internal::DefineBehavior(ptr, ptr),
                  &vectorlEdoublegR_Dictionary, isa_proxy, 0,
                  sizeof(vector<double>) );
      instance.SetNew(&new_vectorlEdoublegR);
      instance.SetNewArray(&newArray_vectorlEdoublegR);
      instance.SetDelete(&delete_vectorlEdoublegR);
      instance.SetDeleteArray(&deleteArray_vectorlEdoublegR);
      instance.SetDestructor(&destruct_vectorlEdoublegR);
      instance.AdoptCollectionProxyInfo(TCollectionProxyInfo::Generate(TCollectionProxyInfo::Pushback< vector<double> >()));

      instance.AdoptAlternate(::ROOT::AddClassAlternate("vector<double>","std::__1::vector<double, std::__1::allocator<double>>"));
      return &instance;
   }
   // Static variable to force the class initialization
   static ::ROOT::TGenericClassInfo *_R__UNIQUE_DICT_(Init) = GenerateInitInstanceLocal(static_cast<const vector<double>*>(nullptr)); R__UseDummy(_R__UNIQUE_DICT_(Init));

   // Dictionary for non-ClassDef classes
   static TClass *vectorlEdoublegR_Dictionary() {
      TClass* theClass =::ROOT::GenerateInitInstanceLocal(static_cast<const vector<double>*>(nullptr))->GetClass();
      vectorlEdoublegR_TClassManip(theClass);
   return theClass;
   }

   static void vectorlEdoublegR_TClassManip(TClass* ){
   }

} // end of namespace ROOT

namespace ROOT {
   // Wrappers around operator new
   static void *new_vectorlEdoublegR(void *p) {
      return  p ? ::new(static_cast<::ROOT::Internal::TOperatorNewHelper*>(p)) vector<double> : new vector<double>;
   }
   static void *newArray_vectorlEdoublegR(Long_t nElements, void *p) {
      return p ? ::new(static_cast<::ROOT::Internal::TOperatorNewHelper*>(p)) vector<double>[nElements] : new vector<double>[nElements];
   }
   // Wrapper around operator delete
   static void delete_vectorlEdoublegR(void *p) {
      delete (static_cast<vector<double>*>(p));
   }
   static void deleteArray_vectorlEdoublegR(void *p) {
      delete [] (static_cast<vector<double>*>(p));
   }
   static void destruct_vectorlEdoublegR(void *p) {
      typedef vector<double> current_t;
      (static_cast<current_t*>(p))->~current_t();
   }
} // end of namespace ROOT for class vector<double>

namespace ROOT {
   static TClass *vectorlEboolgR_Dictionary();
   static void vectorlEboolgR_TClassManip(TClass*);
   static void *new_vectorlEboolgR(void *p = nullptr);
   static void *newArray_vectorlEboolgR(Long_t size, void *p);
   static void delete_vectorlEboolgR(void *p);
   static void deleteArray_vectorlEboolgR(void *p);
   static void destruct_vectorlEboolgR(void *p);

   // Function generating the singleton type initializer
   static TGenericClassInfo *GenerateInitInstanceLocal(const vector<bool>*)
   {
      vector<bool> *ptr = nullptr;
      static ::TVirtualIsAProxy* isa_proxy = new ::TIsAProxy(typeid(vector<bool>));
      static ::ROOT::TGenericClassInfo 
         instance("vector<bool>", -2, "vector", 1859,
                  typeid(vector<bool>), ::ROOT::Internal::DefineBehavior(ptr, ptr),
                  &vectorlEboolgR_Dictionary, isa_proxy, 0,
                  sizeof(vector<bool>) );
      instance.SetNew(&new_vectorlEboolgR);
      instance.SetNewArray(&newArray_vectorlEboolgR);
      instance.SetDelete(&delete_vectorlEboolgR);
      instance.SetDeleteArray(&deleteArray_vectorlEboolgR);
      instance.SetDestructor(&destruct_vectorlEboolgR);
      instance.AdoptCollectionProxyInfo(TCollectionProxyInfo::Generate(TCollectionProxyInfo::Pushback< vector<bool> >()));

      instance.AdoptAlternate(::ROOT::AddClassAlternate("vector<bool>","std::__1::vector<bool, std::__1::allocator<bool>>"));
      return &instance;
   }
   // Static variable to force the class initialization
   static ::ROOT::TGenericClassInfo *_R__UNIQUE_DICT_(Init) = GenerateInitInstanceLocal(static_cast<const vector<bool>*>(nullptr)); R__UseDummy(_R__UNIQUE_DICT_(Init));

   // Dictionary for non-ClassDef classes
   static TClass *vectorlEboolgR_Dictionary() {
      TClass* theClass =::ROOT::GenerateInitInstanceLocal(static_cast<const vector<bool>*>(nullptr))->GetClass();
      vectorlEboolgR_TClassManip(theClass);
   return theClass;
   }

   static void vectorlEboolgR_TClassManip(TClass* ){
   }

} // end of namespace ROOT

namespace ROOT {
   // Wrappers around operator new
   static void *new_vectorlEboolgR(void *p) {
      return  p ? ::new(static_cast<::ROOT::Internal::TOperatorNewHelper*>(p)) vector<bool> : new vector<bool>;
   }
   static void *newArray_vectorlEboolgR(Long_t nElements, void *p) {
      return p ? ::new(static_cast<::ROOT::Internal::TOperatorNewHelper*>(p)) vector<bool>[nElements] : new vector<bool>[nElements];
   }
   // Wrapper around operator delete
   static void delete_vectorlEboolgR(void *p) {
      delete (static_cast<vector<bool>*>(p));
   }
   static void deleteArray_vectorlEboolgR(void *p) {
      delete [] (static_cast<vector<bool>*>(p));
   }
   static void destruct_vectorlEboolgR(void *p) {
      typedef vector<bool> current_t;
      (static_cast<current_t*>(p))->~current_t();
   }
} // end of namespace ROOT for class vector<bool>

namespace ROOT {
   static TClass *maplEstringcOvectorlEintgRsPgR_Dictionary();
   static void maplEstringcOvectorlEintgRsPgR_TClassManip(TClass*);
   static void *new_maplEstringcOvectorlEintgRsPgR(void *p = nullptr);
   static void *newArray_maplEstringcOvectorlEintgRsPgR(Long_t size, void *p);
   static void delete_maplEstringcOvectorlEintgRsPgR(void *p);
   static void deleteArray_maplEstringcOvectorlEintgRsPgR(void *p);
   static void destruct_maplEstringcOvectorlEintgRsPgR(void *p);

   // Function generating the singleton type initializer
   static TGenericClassInfo *GenerateInitInstanceLocal(const map<string,vector<int> >*)
   {
      map<string,vector<int> > *ptr = nullptr;
      static ::TVirtualIsAProxy* isa_proxy = new ::TIsAProxy(typeid(map<string,vector<int> >));
      static ::ROOT::TGenericClassInfo 
         instance("map<string,vector<int> >", -2, "map", 964,
                  typeid(map<string,vector<int> >), ::ROOT::Internal::DefineBehavior(ptr, ptr),
                  &maplEstringcOvectorlEintgRsPgR_Dictionary, isa_proxy, 0,
                  sizeof(map<string,vector<int> >) );
      instance.SetNew(&new_maplEstringcOvectorlEintgRsPgR);
      instance.SetNewArray(&newArray_maplEstringcOvectorlEintgRsPgR);
      instance.SetDelete(&delete_maplEstringcOvectorlEintgRsPgR);
      instance.SetDeleteArray(&deleteArray_maplEstringcOvectorlEintgRsPgR);
      instance.SetDestructor(&destruct_maplEstringcOvectorlEintgRsPgR);
      instance.AdoptCollectionProxyInfo(TCollectionProxyInfo::Generate(TCollectionProxyInfo::MapInsert< map<string,vector<int> > >()));

      instance.AdoptAlternate(::ROOT::AddClassAlternate("map<string,vector<int> >","std::__1::map<std::__1::basic_string<char, std::__1::char_traits<char>, std::__1::allocator<char>>, std::__1::vector<int, std::__1::allocator<int>>, std::__1::less<std::__1::basic_string<char, std::__1::char_traits<char>, std::__1::allocator<char>>>, std::__1::allocator<std::__1::pair<std::__1::basic_string<char, std::__1::char_traits<char>, std::__1::allocator<char>> const, std::__1::vector<int, std::__1::allocator<int>>>>>"));
      return &instance;
   }
   // Static variable to force the class initialization
   static ::ROOT::TGenericClassInfo *_R__UNIQUE_DICT_(Init) = GenerateInitInstanceLocal(static_cast<const map<string,vector<int> >*>(nullptr)); R__UseDummy(_R__UNIQUE_DICT_(Init));

   // Dictionary for non-ClassDef classes
   static TClass *maplEstringcOvectorlEintgRsPgR_Dictionary() {
      TClass* theClass =::ROOT::GenerateInitInstanceLocal(static_cast<const map<string,vector<int> >*>(nullptr))->GetClass();
      maplEstringcOvectorlEintgRsPgR_TClassManip(theClass);
   return theClass;
   }

   static void maplEstringcOvectorlEintgRsPgR_TClassManip(TClass* ){
   }

} // end of namespace ROOT

namespace ROOT {
   // Wrappers around operator new
   static void *new_maplEstringcOvectorlEintgRsPgR(void *p) {
      return  p ? ::new(static_cast<::ROOT::Internal::TOperatorNewHelper*>(p)) map<string,vector<int> > : new map<string,vector<int> >;
   }
   static void *newArray_maplEstringcOvectorlEintgRsPgR(Long_t nElements, void *p) {
      return p ? ::new(static_cast<::ROOT::Internal::TOperatorNewHelper*>(p)) map<string,vector<int> >[nElements] : new map<string,vector<int> >[nElements];
   }
   // Wrapper around operator delete
   static void delete_maplEstringcOvectorlEintgRsPgR(void *p) {
      delete (static_cast<map<string,vector<int> >*>(p));
   }
   static void deleteArray_maplEstringcOvectorlEintgRsPgR(void *p) {
      delete [] (static_cast<map<string,vector<int> >*>(p));
   }
   static void destruct_maplEstringcOvectorlEintgRsPgR(void *p) {
      typedef map<string,vector<int> > current_t;
      (static_cast<current_t*>(p))->~current_t();
   }
} // end of namespace ROOT for class map<string,vector<int> >

namespace ROOT {
   static TClass *maplEstringcOintgR_Dictionary();
   static void maplEstringcOintgR_TClassManip(TClass*);
   static void *new_maplEstringcOintgR(void *p = nullptr);
   static void *newArray_maplEstringcOintgR(Long_t size, void *p);
   static void delete_maplEstringcOintgR(void *p);
   static void deleteArray_maplEstringcOintgR(void *p);
   static void destruct_maplEstringcOintgR(void *p);

   // Function generating the singleton type initializer
   static TGenericClassInfo *GenerateInitInstanceLocal(const map<string,int>*)
   {
      map<string,int> *ptr = nullptr;
      static ::TVirtualIsAProxy* isa_proxy = new ::TIsAProxy(typeid(map<string,int>));
      static ::ROOT::TGenericClassInfo 
         instance("map<string,int>", -2, "map", 964,
                  typeid(map<string,int>), ::ROOT::Internal::DefineBehavior(ptr, ptr),
                  &maplEstringcOintgR_Dictionary, isa_proxy, 0,
                  sizeof(map<string,int>) );
      instance.SetNew(&new_maplEstringcOintgR);
      instance.SetNewArray(&newArray_maplEstringcOintgR);
      instance.SetDelete(&delete_maplEstringcOintgR);
      instance.SetDeleteArray(&deleteArray_maplEstringcOintgR);
      instance.SetDestructor(&destruct_maplEstringcOintgR);
      instance.AdoptCollectionProxyInfo(TCollectionProxyInfo::Generate(TCollectionProxyInfo::MapInsert< map<string,int> >()));

      instance.AdoptAlternate(::ROOT::AddClassAlternate("map<string,int>","std::__1::map<std::__1::basic_string<char, std::__1::char_traits<char>, std::__1::allocator<char>>, int, std::__1::less<std::__1::basic_string<char, std::__1::char_traits<char>, std::__1::allocator<char>>>, std::__1::allocator<std::__1::pair<std::__1::basic_string<char, std::__1::char_traits<char>, std::__1::allocator<char>> const, int>>>"));
      return &instance;
   }
   // Static variable to force the class initialization
   static ::ROOT::TGenericClassInfo *_R__UNIQUE_DICT_(Init) = GenerateInitInstanceLocal(static_cast<const map<string,int>*>(nullptr)); R__UseDummy(_R__UNIQUE_DICT_(Init));

   // Dictionary for non-ClassDef classes
   static TClass *maplEstringcOintgR_Dictionary() {
      TClass* theClass =::ROOT::GenerateInitInstanceLocal(static_cast<const map<string,int>*>(nullptr))->GetClass();
      maplEstringcOintgR_TClassManip(theClass);
   return theClass;
   }

   static void maplEstringcOintgR_TClassManip(TClass* ){
   }

} // end of namespace ROOT

namespace ROOT {
   // Wrappers around operator new
   static void *new_maplEstringcOintgR(void *p) {
      return  p ? ::new(static_cast<::ROOT::Internal::TOperatorNewHelper*>(p)) map<string,int> : new map<string,int>;
   }
   static void *newArray_maplEstringcOintgR(Long_t nElements, void *p) {
      return p ? ::new(static_cast<::ROOT::Internal::TOperatorNewHelper*>(p)) map<string,int>[nElements] : new map<string,int>[nElements];
   }
   // Wrapper around operator delete
   static void delete_maplEstringcOintgR(void *p) {
      delete (static_cast<map<string,int>*>(p));
   }
   static void deleteArray_maplEstringcOintgR(void *p) {
      delete [] (static_cast<map<string,int>*>(p));
   }
   static void destruct_maplEstringcOintgR(void *p) {
      typedef map<string,int> current_t;
      (static_cast<current_t*>(p))->~current_t();
   }
} // end of namespace ROOT for class map<string,int>

namespace ROOT {
   static TClass *maplEstringcOboolgR_Dictionary();
   static void maplEstringcOboolgR_TClassManip(TClass*);
   static void *new_maplEstringcOboolgR(void *p = nullptr);
   static void *newArray_maplEstringcOboolgR(Long_t size, void *p);
   static void delete_maplEstringcOboolgR(void *p);
   static void deleteArray_maplEstringcOboolgR(void *p);
   static void destruct_maplEstringcOboolgR(void *p);

   // Function generating the singleton type initializer
   static TGenericClassInfo *GenerateInitInstanceLocal(const map<string,bool>*)
   {
      map<string,bool> *ptr = nullptr;
      static ::TVirtualIsAProxy* isa_proxy = new ::TIsAProxy(typeid(map<string,bool>));
      static ::ROOT::TGenericClassInfo 
         instance("map<string,bool>", -2, "map", 964,
                  typeid(map<string,bool>), ::ROOT::Internal::DefineBehavior(ptr, ptr),
                  &maplEstringcOboolgR_Dictionary, isa_proxy, 0,
                  sizeof(map<string,bool>) );
      instance.SetNew(&new_maplEstringcOboolgR);
      instance.SetNewArray(&newArray_maplEstringcOboolgR);
      instance.SetDelete(&delete_maplEstringcOboolgR);
      instance.SetDeleteArray(&deleteArray_maplEstringcOboolgR);
      instance.SetDestructor(&destruct_maplEstringcOboolgR);
      instance.AdoptCollectionProxyInfo(TCollectionProxyInfo::Generate(TCollectionProxyInfo::MapInsert< map<string,bool> >()));

      instance.AdoptAlternate(::ROOT::AddClassAlternate("map<string,bool>","std::__1::map<std::__1::basic_string<char, std::__1::char_traits<char>, std::__1::allocator<char>>, bool, std::__1::less<std::__1::basic_string<char, std::__1::char_traits<char>, std::__1::allocator<char>>>, std::__1::allocator<std::__1::pair<std::__1::basic_string<char, std::__1::char_traits<char>, std::__1::allocator<char>> const, bool>>>"));
      return &instance;
   }
   // Static variable to force the class initialization
   static ::ROOT::TGenericClassInfo *_R__UNIQUE_DICT_(Init) = GenerateInitInstanceLocal(static_cast<const map<string,bool>*>(nullptr)); R__UseDummy(_R__UNIQUE_DICT_(Init));

   // Dictionary for non-ClassDef classes
   static TClass *maplEstringcOboolgR_Dictionary() {
      TClass* theClass =::ROOT::GenerateInitInstanceLocal(static_cast<const map<string,bool>*>(nullptr))->GetClass();
      maplEstringcOboolgR_TClassManip(theClass);
   return theClass;
   }

   static void maplEstringcOboolgR_TClassManip(TClass* ){
   }

} // end of namespace ROOT

namespace ROOT {
   // Wrappers around operator new
   static void *new_maplEstringcOboolgR(void *p) {
      return  p ? ::new(static_cast<::ROOT::Internal::TOperatorNewHelper*>(p)) map<string,bool> : new map<string,bool>;
   }
   static void *newArray_maplEstringcOboolgR(Long_t nElements, void *p) {
      return p ? ::new(static_cast<::ROOT::Internal::TOperatorNewHelper*>(p)) map<string,bool>[nElements] : new map<string,bool>[nElements];
   }
   // Wrapper around operator delete
   static void delete_maplEstringcOboolgR(void *p) {
      delete (static_cast<map<string,bool>*>(p));
   }
   static void deleteArray_maplEstringcOboolgR(void *p) {
      delete [] (static_cast<map<string,bool>*>(p));
   }
   static void destruct_maplEstringcOboolgR(void *p) {
      typedef map<string,bool> current_t;
      (static_cast<current_t*>(p))->~current_t();
   }
} // end of namespace ROOT for class map<string,bool>

namespace ROOT {
   static TClass *maplEstringcOTChainmUgR_Dictionary();
   static void maplEstringcOTChainmUgR_TClassManip(TClass*);
   static void *new_maplEstringcOTChainmUgR(void *p = nullptr);
   static void *newArray_maplEstringcOTChainmUgR(Long_t size, void *p);
   static void delete_maplEstringcOTChainmUgR(void *p);
   static void deleteArray_maplEstringcOTChainmUgR(void *p);
   static void destruct_maplEstringcOTChainmUgR(void *p);

   // Function generating the singleton type initializer
   static TGenericClassInfo *GenerateInitInstanceLocal(const map<string,TChain*>*)
   {
      map<string,TChain*> *ptr = nullptr;
      static ::TVirtualIsAProxy* isa_proxy = new ::TIsAProxy(typeid(map<string,TChain*>));
      static ::ROOT::TGenericClassInfo 
         instance("map<string,TChain*>", -2, "map", 964,
                  typeid(map<string,TChain*>), ::ROOT::Internal::DefineBehavior(ptr, ptr),
                  &maplEstringcOTChainmUgR_Dictionary, isa_proxy, 0,
                  sizeof(map<string,TChain*>) );
      instance.SetNew(&new_maplEstringcOTChainmUgR);
      instance.SetNewArray(&newArray_maplEstringcOTChainmUgR);
      instance.SetDelete(&delete_maplEstringcOTChainmUgR);
      instance.SetDeleteArray(&deleteArray_maplEstringcOTChainmUgR);
      instance.SetDestructor(&destruct_maplEstringcOTChainmUgR);
      instance.AdoptCollectionProxyInfo(TCollectionProxyInfo::Generate(TCollectionProxyInfo::MapInsert< map<string,TChain*> >()));

      instance.AdoptAlternate(::ROOT::AddClassAlternate("map<string,TChain*>","std::__1::map<std::__1::basic_string<char, std::__1::char_traits<char>, std::__1::allocator<char>>, TChain*, std::__1::less<std::__1::basic_string<char, std::__1::char_traits<char>, std::__1::allocator<char>>>, std::__1::allocator<std::__1::pair<std::__1::basic_string<char, std::__1::char_traits<char>, std::__1::allocator<char>> const, TChain*>>>"));
      return &instance;
   }
   // Static variable to force the class initialization
   static ::ROOT::TGenericClassInfo *_R__UNIQUE_DICT_(Init) = GenerateInitInstanceLocal(static_cast<const map<string,TChain*>*>(nullptr)); R__UseDummy(_R__UNIQUE_DICT_(Init));

   // Dictionary for non-ClassDef classes
   static TClass *maplEstringcOTChainmUgR_Dictionary() {
      TClass* theClass =::ROOT::GenerateInitInstanceLocal(static_cast<const map<string,TChain*>*>(nullptr))->GetClass();
      maplEstringcOTChainmUgR_TClassManip(theClass);
   return theClass;
   }

   static void maplEstringcOTChainmUgR_TClassManip(TClass* ){
   }

} // end of namespace ROOT

namespace ROOT {
   // Wrappers around operator new
   static void *new_maplEstringcOTChainmUgR(void *p) {
      return  p ? ::new(static_cast<::ROOT::Internal::TOperatorNewHelper*>(p)) map<string,TChain*> : new map<string,TChain*>;
   }
   static void *newArray_maplEstringcOTChainmUgR(Long_t nElements, void *p) {
      return p ? ::new(static_cast<::ROOT::Internal::TOperatorNewHelper*>(p)) map<string,TChain*>[nElements] : new map<string,TChain*>[nElements];
   }
   // Wrapper around operator delete
   static void delete_maplEstringcOTChainmUgR(void *p) {
      delete (static_cast<map<string,TChain*>*>(p));
   }
   static void deleteArray_maplEstringcOTChainmUgR(void *p) {
      delete [] (static_cast<map<string,TChain*>*>(p));
   }
   static void destruct_maplEstringcOTChainmUgR(void *p) {
      typedef map<string,TChain*> current_t;
      (static_cast<current_t*>(p))->~current_t();
   }
} // end of namespace ROOT for class map<string,TChain*>

namespace ROOT {
   static TClass *maplEstringcOFieldgR_Dictionary();
   static void maplEstringcOFieldgR_TClassManip(TClass*);
   static void *new_maplEstringcOFieldgR(void *p = nullptr);
   static void *newArray_maplEstringcOFieldgR(Long_t size, void *p);
   static void delete_maplEstringcOFieldgR(void *p);
   static void deleteArray_maplEstringcOFieldgR(void *p);
   static void destruct_maplEstringcOFieldgR(void *p);

   // Function generating the singleton type initializer
   static TGenericClassInfo *GenerateInitInstanceLocal(const map<string,Field>*)
   {
      map<string,Field> *ptr = nullptr;
      static ::TVirtualIsAProxy* isa_proxy = new ::TIsAProxy(typeid(map<string,Field>));
      static ::ROOT::TGenericClassInfo 
         instance("map<string,Field>", -2, "map", 964,
                  typeid(map<string,Field>), ::ROOT::Internal::DefineBehavior(ptr, ptr),
                  &maplEstringcOFieldgR_Dictionary, isa_proxy, 0,
                  sizeof(map<string,Field>) );
      instance.SetNew(&new_maplEstringcOFieldgR);
      instance.SetNewArray(&newArray_maplEstringcOFieldgR);
      instance.SetDelete(&delete_maplEstringcOFieldgR);
      instance.SetDeleteArray(&deleteArray_maplEstringcOFieldgR);
      instance.SetDestructor(&destruct_maplEstringcOFieldgR);
      instance.AdoptCollectionProxyInfo(TCollectionProxyInfo::Generate(TCollectionProxyInfo::MapInsert< map<string,Field> >()));

      instance.AdoptAlternate(::ROOT::AddClassAlternate("map<string,Field>","std::__1::map<std::__1::basic_string<char, std::__1::char_traits<char>, std::__1::allocator<char>>, Field, std::__1::less<std::__1::basic_string<char, std::__1::char_traits<char>, std::__1::allocator<char>>>, std::__1::allocator<std::__1::pair<std::__1::basic_string<char, std::__1::char_traits<char>, std::__1::allocator<char>> const, Field>>>"));
      return &instance;
   }
   // Static variable to force the class initialization
   static ::ROOT::TGenericClassInfo *_R__UNIQUE_DICT_(Init) = GenerateInitInstanceLocal(static_cast<const map<string,Field>*>(nullptr)); R__UseDummy(_R__UNIQUE_DICT_(Init));

   // Dictionary for non-ClassDef classes
   static TClass *maplEstringcOFieldgR_Dictionary() {
      TClass* theClass =::ROOT::GenerateInitInstanceLocal(static_cast<const map<string,Field>*>(nullptr))->GetClass();
      maplEstringcOFieldgR_TClassManip(theClass);
   return theClass;
   }

   static void maplEstringcOFieldgR_TClassManip(TClass* ){
   }

} // end of namespace ROOT

namespace ROOT {
   // Wrappers around operator new
   static void *new_maplEstringcOFieldgR(void *p) {
      return  p ? ::new(static_cast<::ROOT::Internal::TOperatorNewHelper*>(p)) map<string,Field> : new map<string,Field>;
   }
   static void *newArray_maplEstringcOFieldgR(Long_t nElements, void *p) {
      return p ? ::new(static_cast<::ROOT::Internal::TOperatorNewHelper*>(p)) map<string,Field>[nElements] : new map<string,Field>[nElements];
   }
   // Wrapper around operator delete
   static void delete_maplEstringcOFieldgR(void *p) {
      delete (static_cast<map<string,Field>*>(p));
   }
   static void deleteArray_maplEstringcOFieldgR(void *p) {
      delete [] (static_cast<map<string,Field>*>(p));
   }
   static void destruct_maplEstringcOFieldgR(void *p) {
      typedef map<string,Field> current_t;
      (static_cast<current_t*>(p))->~current_t();
   }
} // end of namespace ROOT for class map<string,Field>

namespace ROOT {
   static TClass *maplEstringcOFieldmUgR_Dictionary();
   static void maplEstringcOFieldmUgR_TClassManip(TClass*);
   static void *new_maplEstringcOFieldmUgR(void *p = nullptr);
   static void *newArray_maplEstringcOFieldmUgR(Long_t size, void *p);
   static void delete_maplEstringcOFieldmUgR(void *p);
   static void deleteArray_maplEstringcOFieldmUgR(void *p);
   static void destruct_maplEstringcOFieldmUgR(void *p);

   // Function generating the singleton type initializer
   static TGenericClassInfo *GenerateInitInstanceLocal(const map<string,Field*>*)
   {
      map<string,Field*> *ptr = nullptr;
      static ::TVirtualIsAProxy* isa_proxy = new ::TIsAProxy(typeid(map<string,Field*>));
      static ::ROOT::TGenericClassInfo 
         instance("map<string,Field*>", -2, "map", 964,
                  typeid(map<string,Field*>), ::ROOT::Internal::DefineBehavior(ptr, ptr),
                  &maplEstringcOFieldmUgR_Dictionary, isa_proxy, 0,
                  sizeof(map<string,Field*>) );
      instance.SetNew(&new_maplEstringcOFieldmUgR);
      instance.SetNewArray(&newArray_maplEstringcOFieldmUgR);
      instance.SetDelete(&delete_maplEstringcOFieldmUgR);
      instance.SetDeleteArray(&deleteArray_maplEstringcOFieldmUgR);
      instance.SetDestructor(&destruct_maplEstringcOFieldmUgR);
      instance.AdoptCollectionProxyInfo(TCollectionProxyInfo::Generate(TCollectionProxyInfo::MapInsert< map<string,Field*> >()));

      instance.AdoptAlternate(::ROOT::AddClassAlternate("map<string,Field*>","std::__1::map<std::__1::basic_string<char, std::__1::char_traits<char>, std::__1::allocator<char>>, Field*, std::__1::less<std::__1::basic_string<char, std::__1::char_traits<char>, std::__1::allocator<char>>>, std::__1::allocator<std::__1::pair<std::__1::basic_string<char, std::__1::char_traits<char>, std::__1::allocator<char>> const, Field*>>>"));
      return &instance;
   }
   // Static variable to force the class initialization
   static ::ROOT::TGenericClassInfo *_R__UNIQUE_DICT_(Init) = GenerateInitInstanceLocal(static_cast<const map<string,Field*>*>(nullptr)); R__UseDummy(_R__UNIQUE_DICT_(Init));

   // Dictionary for non-ClassDef classes
   static TClass *maplEstringcOFieldmUgR_Dictionary() {
      TClass* theClass =::ROOT::GenerateInitInstanceLocal(static_cast<const map<string,Field*>*>(nullptr))->GetClass();
      maplEstringcOFieldmUgR_TClassManip(theClass);
   return theClass;
   }

   static void maplEstringcOFieldmUgR_TClassManip(TClass* ){
   }

} // end of namespace ROOT

namespace ROOT {
   // Wrappers around operator new
   static void *new_maplEstringcOFieldmUgR(void *p) {
      return  p ? ::new(static_cast<::ROOT::Internal::TOperatorNewHelper*>(p)) map<string,Field*> : new map<string,Field*>;
   }
   static void *newArray_maplEstringcOFieldmUgR(Long_t nElements, void *p) {
      return p ? ::new(static_cast<::ROOT::Internal::TOperatorNewHelper*>(p)) map<string,Field*>[nElements] : new map<string,Field*>[nElements];
   }
   // Wrapper around operator delete
   static void delete_maplEstringcOFieldmUgR(void *p) {
      delete (static_cast<map<string,Field*>*>(p));
   }
   static void deleteArray_maplEstringcOFieldmUgR(void *p) {
      delete [] (static_cast<map<string,Field*>*>(p));
   }
   static void destruct_maplEstringcOFieldmUgR(void *p) {
      typedef map<string,Field*> current_t;
      (static_cast<current_t*>(p))->~current_t();
   }
} // end of namespace ROOT for class map<string,Field*>

namespace {
  void TriggerDictionaryInitialization_dictionaryForROOT_Impl() {
    static const char* headers[] = {
"include/tnm.h",
nullptr
    };
    static const char* includePaths[] = {
"/opt/homebrew/include",
"/opt/homebrew/Cellar/root/6.34.08_1/include/root",
"/Users/jhlee/tempTTHH/",
nullptr
    };
    static const char* fwdDeclCode = R"DICTFWDDCLS(
#line 1 "dictionaryForROOT dictionary forward declarations' payload"
#pragma clang diagnostic ignored "-Wkeyword-compat"
#pragma clang diagnostic ignored "-Wignored-attributes"
#pragma clang diagnostic ignored "-Wreturn-type-c-linkage"
extern int __Cling_AutoLoading_Map;
class __attribute__((annotate("$clingAutoload$treestream.h")))  __attribute__((annotate("$clingAutoload$include/tnm.h")))  itreestream;
class __attribute__((annotate("$clingAutoload$treestream.h")))  __attribute__((annotate("$clingAutoload$include/tnm.h")))  otreestream;
struct __attribute__((annotate("$clingAutoload$eventBuffer.h")))  __attribute__((annotate("$clingAutoload$include/tnm.h")))  eventBuffer;
struct __attribute__((annotate("$clingAutoload$include/tnm.h")))  outputFile;
struct __attribute__((annotate("$clingAutoload$include/tnm.h")))  commandLine;
struct __attribute__((annotate("$clingAutoload$include/tnm.h")))  matchedPair;
struct __attribute__((annotate("$clingAutoload$include/tnm.h")))  ptThing;
)DICTFWDDCLS";
    static const char* payloadCode = R"DICTPAYLOAD(
#line 1 "dictionaryForROOT dictionary payload"


#define _BACKWARD_BACKWARD_WARNING_H
// Inline headers
#include "include/tnm.h"

#undef  _BACKWARD_BACKWARD_WARNING_H
)DICTPAYLOAD";
    static const char* classesHeaders[] = {
"change", payloadCode, "@",
"commandLine", payloadCode, "@",
"deltaPhi", payloadCode, "@",
"deltaR", payloadCode, "@",
"error", payloadCode, "@",
"eventBuffer", payloadCode, "@",
"eventBuffer::CorrT1METJet_s", payloadCode, "@",
"eventBuffer::Electron_s", payloadCode, "@",
"eventBuffer::FatJet_s", payloadCode, "@",
"eventBuffer::FsrPhoton_s", payloadCode, "@",
"eventBuffer::GenIsolatedPhoton_s", payloadCode, "@",
"eventBuffer::GenJetAK8_s", payloadCode, "@",
"eventBuffer::GenJet_s", payloadCode, "@",
"eventBuffer::GenPart_s", payloadCode, "@",
"eventBuffer::IsoTrack_s", payloadCode, "@",
"eventBuffer::Jet_s", payloadCode, "@",
"eventBuffer::LHEPart_s", payloadCode, "@",
"eventBuffer::Muon_s", payloadCode, "@",
"eventBuffer::Photon_s", payloadCode, "@",
"eventBuffer::SubGenJetAK8_s", payloadCode, "@",
"eventBuffer::SubJet_s", payloadCode, "@",
"eventBuffer::Tau_s", payloadCode, "@",
"eventBuffer::TrigObj_s", payloadCode, "@",
"eventBuffer::boostedTau_s", payloadCode, "@",
"fileNames", payloadCode, "@",
"itreestream", payloadCode, "@",
"matchedPair", payloadCode, "@",
"nameonly", payloadCode, "@",
"otreestream", payloadCode, "@",
"outputFile", payloadCode, "@",
"particleName", payloadCode, "@",
"ptThing", payloadCode, "@",
"setStyle", payloadCode, "@",
"shell", payloadCode, "@",
"split", payloadCode, "@",
"strip", payloadCode, "@",
nullptr
};
    static bool isInitialized = false;
    if (!isInitialized) {
      TROOT::RegisterModule("dictionaryForROOT",
        headers, includePaths, payloadCode, fwdDeclCode,
        TriggerDictionaryInitialization_dictionaryForROOT_Impl, {}, classesHeaders, /*hasCxxModule*/false);
      isInitialized = true;
    }
  }
  static struct DictInit {
    DictInit() {
      TriggerDictionaryInitialization_dictionaryForROOT_Impl();
    }
  } __TheDictionaryInitializer;
}
void TriggerDictionaryInitialization_dictionaryForROOT() {
  TriggerDictionaryInitialization_dictionaryForROOT_Impl();
}
