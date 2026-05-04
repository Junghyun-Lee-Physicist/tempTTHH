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
   static TClass *itreestream_Dictionary();
   static void itreestream_TClassManip(TClass*);
   static void *new_itreestream(void *p = nullptr);
   static void *newArray_itreestream(Long_t size, void *p);
   static void delete_itreestream(void *p);
   static void deleteArray_itreestream(void *p);
   static void destruct_itreestream(void *p);

   // Function generating the singleton type initializer
   static TGenericClassInfo *GenerateInitInstanceLocal(const ::itreestream*)
   {
      ::itreestream *ptr = nullptr;
      static ::TVirtualIsAProxy* isa_proxy = new ::TIsAProxy(typeid(::itreestream));
      static ::ROOT::TGenericClassInfo 
         instance("itreestream", "treestream.h", 159,
                  typeid(::itreestream), ::ROOT::Internal::DefineBehavior(ptr, ptr),
                  &itreestream_Dictionary, isa_proxy, 4,
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

   // Dictionary for non-ClassDef classes
   static TClass *itreestream_Dictionary() {
      TClass* theClass =::ROOT::GenerateInitInstanceLocal(static_cast<const ::itreestream*>(nullptr))->GetClass();
      itreestream_TClassManip(theClass);
   return theClass;
   }

   static void itreestream_TClassManip(TClass* ){
   }

} // end of namespace ROOT

namespace ROOT {
   static TClass *otreestream_Dictionary();
   static void otreestream_TClassManip(TClass*);
   static void *new_otreestream(void *p = nullptr);
   static void *newArray_otreestream(Long_t size, void *p);
   static void delete_otreestream(void *p);
   static void deleteArray_otreestream(void *p);
   static void destruct_otreestream(void *p);

   // Function generating the singleton type initializer
   static TGenericClassInfo *GenerateInitInstanceLocal(const ::otreestream*)
   {
      ::otreestream *ptr = nullptr;
      static ::TVirtualIsAProxy* isa_proxy = new ::TIsAProxy(typeid(::otreestream));
      static ::ROOT::TGenericClassInfo 
         instance("otreestream", "treestream.h", 404,
                  typeid(::otreestream), ::ROOT::Internal::DefineBehavior(ptr, ptr),
                  &otreestream_Dictionary, isa_proxy, 4,
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

   // Dictionary for non-ClassDef classes
   static TClass *otreestream_Dictionary() {
      TClass* theClass =::ROOT::GenerateInitInstanceLocal(static_cast<const ::otreestream*>(nullptr))->GetClass();
      otreestream_TClassManip(theClass);
   return theClass;
   }

   static void otreestream_TClassManip(TClass* ){
   }

} // end of namespace ROOT

namespace ROOT {
   static TClass *eventBuffer_Dictionary();
   static void eventBuffer_TClassManip(TClass*);
   static void *new_eventBuffer(void *p = nullptr);
   static void *newArray_eventBuffer(Long_t size, void *p);
   static void delete_eventBuffer(void *p);
   static void deleteArray_eventBuffer(void *p);
   static void destruct_eventBuffer(void *p);

   // Function generating the singleton type initializer
   static TGenericClassInfo *GenerateInitInstanceLocal(const ::eventBuffer*)
   {
      ::eventBuffer *ptr = nullptr;
      static ::TVirtualIsAProxy* isa_proxy = new ::TIsAProxy(typeid(::eventBuffer));
      static ::ROOT::TGenericClassInfo 
         instance("eventBuffer", "eventBuffer.h", 22,
                  typeid(::eventBuffer), ::ROOT::Internal::DefineBehavior(ptr, ptr),
                  &eventBuffer_Dictionary, isa_proxy, 4,
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

   // Dictionary for non-ClassDef classes
   static TClass *eventBuffer_Dictionary() {
      TClass* theClass =::ROOT::GenerateInitInstanceLocal(static_cast<const ::eventBuffer*>(nullptr))->GetClass();
      eventBuffer_TClassManip(theClass);
   return theClass;
   }

   static void eventBuffer_TClassManip(TClass* ){
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
         instance("eventBuffer::Electron_s", "eventBuffer.h", 443,
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
         instance("eventBuffer::FatJet_s", "eventBuffer.h", 484,
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
         instance("eventBuffer::GenJet_s", "eventBuffer.h", 573,
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
         instance("eventBuffer::GenPart_s", "eventBuffer.h", 596,
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
         instance("eventBuffer::Jet_s", "eventBuffer.h", 623,
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
         instance("eventBuffer::Muon_s", "eventBuffer.h", 670,
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
         instance("outputFile", "tnm.h", 27,
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
         instance("commandLine", "tnm.h", 46,
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
         instance("matchedPair", "tnm.h", 83,
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
         instance("ptThing", "tnm.h", 93,
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
         instance("vector<string>", -2, "vector", 423,
                  typeid(vector<string>), ::ROOT::Internal::DefineBehavior(ptr, ptr),
                  &vectorlEstringgR_Dictionary, isa_proxy, 0,
                  sizeof(vector<string>) );
      instance.SetNew(&new_vectorlEstringgR);
      instance.SetNewArray(&newArray_vectorlEstringgR);
      instance.SetDelete(&delete_vectorlEstringgR);
      instance.SetDeleteArray(&deleteArray_vectorlEstringgR);
      instance.SetDestructor(&destruct_vectorlEstringgR);
      instance.AdoptCollectionProxyInfo(TCollectionProxyInfo::Generate(TCollectionProxyInfo::Pushback< vector<string> >()));

      instance.AdoptAlternate(::ROOT::AddClassAlternate("vector<string>","std::vector<std::__cxx11::basic_string<char, std::char_traits<char>, std::allocator<char> >, std::allocator<std::__cxx11::basic_string<char, std::char_traits<char>, std::allocator<char> > > >"));
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
         instance("vector<int>", -2, "vector", 423,
                  typeid(vector<int>), ::ROOT::Internal::DefineBehavior(ptr, ptr),
                  &vectorlEintgR_Dictionary, isa_proxy, 0,
                  sizeof(vector<int>) );
      instance.SetNew(&new_vectorlEintgR);
      instance.SetNewArray(&newArray_vectorlEintgR);
      instance.SetDelete(&delete_vectorlEintgR);
      instance.SetDeleteArray(&deleteArray_vectorlEintgR);
      instance.SetDestructor(&destruct_vectorlEintgR);
      instance.AdoptCollectionProxyInfo(TCollectionProxyInfo::Generate(TCollectionProxyInfo::Pushback< vector<int> >()));

      instance.AdoptAlternate(::ROOT::AddClassAlternate("vector<int>","std::vector<int, std::allocator<int> >"));
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
         instance("vector<int*>", -2, "vector", 423,
                  typeid(vector<int*>), ::ROOT::Internal::DefineBehavior(ptr, ptr),
                  &vectorlEintmUgR_Dictionary, isa_proxy, 0,
                  sizeof(vector<int*>) );
      instance.SetNew(&new_vectorlEintmUgR);
      instance.SetNewArray(&newArray_vectorlEintmUgR);
      instance.SetDelete(&delete_vectorlEintmUgR);
      instance.SetDeleteArray(&deleteArray_vectorlEintmUgR);
      instance.SetDestructor(&destruct_vectorlEintmUgR);
      instance.AdoptCollectionProxyInfo(TCollectionProxyInfo::Generate(TCollectionProxyInfo::Pushback< vector<int*> >()));

      instance.AdoptAlternate(::ROOT::AddClassAlternate("vector<int*>","std::vector<int*, std::allocator<int*> >"));
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
         instance("vector<float>", -2, "vector", 423,
                  typeid(vector<float>), ::ROOT::Internal::DefineBehavior(ptr, ptr),
                  &vectorlEfloatgR_Dictionary, isa_proxy, 0,
                  sizeof(vector<float>) );
      instance.SetNew(&new_vectorlEfloatgR);
      instance.SetNewArray(&newArray_vectorlEfloatgR);
      instance.SetDelete(&delete_vectorlEfloatgR);
      instance.SetDeleteArray(&deleteArray_vectorlEfloatgR);
      instance.SetDestructor(&destruct_vectorlEfloatgR);
      instance.AdoptCollectionProxyInfo(TCollectionProxyInfo::Generate(TCollectionProxyInfo::Pushback< vector<float> >()));

      instance.AdoptAlternate(::ROOT::AddClassAlternate("vector<float>","std::vector<float, std::allocator<float> >"));
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
         instance("vector<eventBuffer::Muon_s>", -2, "vector", 423,
                  typeid(vector<eventBuffer::Muon_s>), ::ROOT::Internal::DefineBehavior(ptr, ptr),
                  &vectorlEeventBuffercLcLMuon_sgR_Dictionary, isa_proxy, 4,
                  sizeof(vector<eventBuffer::Muon_s>) );
      instance.SetNew(&new_vectorlEeventBuffercLcLMuon_sgR);
      instance.SetNewArray(&newArray_vectorlEeventBuffercLcLMuon_sgR);
      instance.SetDelete(&delete_vectorlEeventBuffercLcLMuon_sgR);
      instance.SetDeleteArray(&deleteArray_vectorlEeventBuffercLcLMuon_sgR);
      instance.SetDestructor(&destruct_vectorlEeventBuffercLcLMuon_sgR);
      instance.AdoptCollectionProxyInfo(TCollectionProxyInfo::Generate(TCollectionProxyInfo::Pushback< vector<eventBuffer::Muon_s> >()));

      instance.AdoptAlternate(::ROOT::AddClassAlternate("vector<eventBuffer::Muon_s>","std::vector<eventBuffer::Muon_s, std::allocator<eventBuffer::Muon_s> >"));
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
         instance("vector<eventBuffer::Jet_s>", -2, "vector", 423,
                  typeid(vector<eventBuffer::Jet_s>), ::ROOT::Internal::DefineBehavior(ptr, ptr),
                  &vectorlEeventBuffercLcLJet_sgR_Dictionary, isa_proxy, 4,
                  sizeof(vector<eventBuffer::Jet_s>) );
      instance.SetNew(&new_vectorlEeventBuffercLcLJet_sgR);
      instance.SetNewArray(&newArray_vectorlEeventBuffercLcLJet_sgR);
      instance.SetDelete(&delete_vectorlEeventBuffercLcLJet_sgR);
      instance.SetDeleteArray(&deleteArray_vectorlEeventBuffercLcLJet_sgR);
      instance.SetDestructor(&destruct_vectorlEeventBuffercLcLJet_sgR);
      instance.AdoptCollectionProxyInfo(TCollectionProxyInfo::Generate(TCollectionProxyInfo::Pushback< vector<eventBuffer::Jet_s> >()));

      instance.AdoptAlternate(::ROOT::AddClassAlternate("vector<eventBuffer::Jet_s>","std::vector<eventBuffer::Jet_s, std::allocator<eventBuffer::Jet_s> >"));
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
         instance("vector<eventBuffer::GenPart_s>", -2, "vector", 423,
                  typeid(vector<eventBuffer::GenPart_s>), ::ROOT::Internal::DefineBehavior(ptr, ptr),
                  &vectorlEeventBuffercLcLGenPart_sgR_Dictionary, isa_proxy, 4,
                  sizeof(vector<eventBuffer::GenPart_s>) );
      instance.SetNew(&new_vectorlEeventBuffercLcLGenPart_sgR);
      instance.SetNewArray(&newArray_vectorlEeventBuffercLcLGenPart_sgR);
      instance.SetDelete(&delete_vectorlEeventBuffercLcLGenPart_sgR);
      instance.SetDeleteArray(&deleteArray_vectorlEeventBuffercLcLGenPart_sgR);
      instance.SetDestructor(&destruct_vectorlEeventBuffercLcLGenPart_sgR);
      instance.AdoptCollectionProxyInfo(TCollectionProxyInfo::Generate(TCollectionProxyInfo::Pushback< vector<eventBuffer::GenPart_s> >()));

      instance.AdoptAlternate(::ROOT::AddClassAlternate("vector<eventBuffer::GenPart_s>","std::vector<eventBuffer::GenPart_s, std::allocator<eventBuffer::GenPart_s> >"));
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
         instance("vector<eventBuffer::GenJet_s>", -2, "vector", 423,
                  typeid(vector<eventBuffer::GenJet_s>), ::ROOT::Internal::DefineBehavior(ptr, ptr),
                  &vectorlEeventBuffercLcLGenJet_sgR_Dictionary, isa_proxy, 4,
                  sizeof(vector<eventBuffer::GenJet_s>) );
      instance.SetNew(&new_vectorlEeventBuffercLcLGenJet_sgR);
      instance.SetNewArray(&newArray_vectorlEeventBuffercLcLGenJet_sgR);
      instance.SetDelete(&delete_vectorlEeventBuffercLcLGenJet_sgR);
      instance.SetDeleteArray(&deleteArray_vectorlEeventBuffercLcLGenJet_sgR);
      instance.SetDestructor(&destruct_vectorlEeventBuffercLcLGenJet_sgR);
      instance.AdoptCollectionProxyInfo(TCollectionProxyInfo::Generate(TCollectionProxyInfo::Pushback< vector<eventBuffer::GenJet_s> >()));

      instance.AdoptAlternate(::ROOT::AddClassAlternate("vector<eventBuffer::GenJet_s>","std::vector<eventBuffer::GenJet_s, std::allocator<eventBuffer::GenJet_s> >"));
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
         instance("vector<eventBuffer::FatJet_s>", -2, "vector", 423,
                  typeid(vector<eventBuffer::FatJet_s>), ::ROOT::Internal::DefineBehavior(ptr, ptr),
                  &vectorlEeventBuffercLcLFatJet_sgR_Dictionary, isa_proxy, 4,
                  sizeof(vector<eventBuffer::FatJet_s>) );
      instance.SetNew(&new_vectorlEeventBuffercLcLFatJet_sgR);
      instance.SetNewArray(&newArray_vectorlEeventBuffercLcLFatJet_sgR);
      instance.SetDelete(&delete_vectorlEeventBuffercLcLFatJet_sgR);
      instance.SetDeleteArray(&deleteArray_vectorlEeventBuffercLcLFatJet_sgR);
      instance.SetDestructor(&destruct_vectorlEeventBuffercLcLFatJet_sgR);
      instance.AdoptCollectionProxyInfo(TCollectionProxyInfo::Generate(TCollectionProxyInfo::Pushback< vector<eventBuffer::FatJet_s> >()));

      instance.AdoptAlternate(::ROOT::AddClassAlternate("vector<eventBuffer::FatJet_s>","std::vector<eventBuffer::FatJet_s, std::allocator<eventBuffer::FatJet_s> >"));
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
         instance("vector<eventBuffer::Electron_s>", -2, "vector", 423,
                  typeid(vector<eventBuffer::Electron_s>), ::ROOT::Internal::DefineBehavior(ptr, ptr),
                  &vectorlEeventBuffercLcLElectron_sgR_Dictionary, isa_proxy, 4,
                  sizeof(vector<eventBuffer::Electron_s>) );
      instance.SetNew(&new_vectorlEeventBuffercLcLElectron_sgR);
      instance.SetNewArray(&newArray_vectorlEeventBuffercLcLElectron_sgR);
      instance.SetDelete(&delete_vectorlEeventBuffercLcLElectron_sgR);
      instance.SetDeleteArray(&deleteArray_vectorlEeventBuffercLcLElectron_sgR);
      instance.SetDestructor(&destruct_vectorlEeventBuffercLcLElectron_sgR);
      instance.AdoptCollectionProxyInfo(TCollectionProxyInfo::Generate(TCollectionProxyInfo::Pushback< vector<eventBuffer::Electron_s> >()));

      instance.AdoptAlternate(::ROOT::AddClassAlternate("vector<eventBuffer::Electron_s>","std::vector<eventBuffer::Electron_s, std::allocator<eventBuffer::Electron_s> >"));
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
         instance("vector<double>", -2, "vector", 423,
                  typeid(vector<double>), ::ROOT::Internal::DefineBehavior(ptr, ptr),
                  &vectorlEdoublegR_Dictionary, isa_proxy, 0,
                  sizeof(vector<double>) );
      instance.SetNew(&new_vectorlEdoublegR);
      instance.SetNewArray(&newArray_vectorlEdoublegR);
      instance.SetDelete(&delete_vectorlEdoublegR);
      instance.SetDeleteArray(&deleteArray_vectorlEdoublegR);
      instance.SetDestructor(&destruct_vectorlEdoublegR);
      instance.AdoptCollectionProxyInfo(TCollectionProxyInfo::Generate(TCollectionProxyInfo::Pushback< vector<double> >()));

      instance.AdoptAlternate(::ROOT::AddClassAlternate("vector<double>","std::vector<double, std::allocator<double> >"));
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
         instance("vector<bool>", -2, "vector", 690,
                  typeid(vector<bool>), ::ROOT::Internal::DefineBehavior(ptr, ptr),
                  &vectorlEboolgR_Dictionary, isa_proxy, 0,
                  sizeof(vector<bool>) );
      instance.SetNew(&new_vectorlEboolgR);
      instance.SetNewArray(&newArray_vectorlEboolgR);
      instance.SetDelete(&delete_vectorlEboolgR);
      instance.SetDeleteArray(&deleteArray_vectorlEboolgR);
      instance.SetDestructor(&destruct_vectorlEboolgR);
      instance.AdoptCollectionProxyInfo(TCollectionProxyInfo::Generate(TCollectionProxyInfo::Pushback< vector<bool> >()));

      instance.AdoptAlternate(::ROOT::AddClassAlternate("vector<bool>","std::vector<bool, std::allocator<bool> >"));
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
         instance("map<string,vector<int> >", -2, "map", 100,
                  typeid(map<string,vector<int> >), ::ROOT::Internal::DefineBehavior(ptr, ptr),
                  &maplEstringcOvectorlEintgRsPgR_Dictionary, isa_proxy, 0,
                  sizeof(map<string,vector<int> >) );
      instance.SetNew(&new_maplEstringcOvectorlEintgRsPgR);
      instance.SetNewArray(&newArray_maplEstringcOvectorlEintgRsPgR);
      instance.SetDelete(&delete_maplEstringcOvectorlEintgRsPgR);
      instance.SetDeleteArray(&deleteArray_maplEstringcOvectorlEintgRsPgR);
      instance.SetDestructor(&destruct_maplEstringcOvectorlEintgRsPgR);
      instance.AdoptCollectionProxyInfo(TCollectionProxyInfo::Generate(TCollectionProxyInfo::MapInsert< map<string,vector<int> > >()));

      instance.AdoptAlternate(::ROOT::AddClassAlternate("map<string,vector<int> >","std::map<std::__cxx11::basic_string<char, std::char_traits<char>, std::allocator<char> >, std::vector<int, std::allocator<int> >, std::less<std::__cxx11::basic_string<char, std::char_traits<char>, std::allocator<char> > >, std::allocator<std::pair<std::__cxx11::basic_string<char, std::char_traits<char>, std::allocator<char> > const, std::vector<int, std::allocator<int> > > > >"));
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
         instance("map<string,int>", -2, "map", 100,
                  typeid(map<string,int>), ::ROOT::Internal::DefineBehavior(ptr, ptr),
                  &maplEstringcOintgR_Dictionary, isa_proxy, 0,
                  sizeof(map<string,int>) );
      instance.SetNew(&new_maplEstringcOintgR);
      instance.SetNewArray(&newArray_maplEstringcOintgR);
      instance.SetDelete(&delete_maplEstringcOintgR);
      instance.SetDeleteArray(&deleteArray_maplEstringcOintgR);
      instance.SetDestructor(&destruct_maplEstringcOintgR);
      instance.AdoptCollectionProxyInfo(TCollectionProxyInfo::Generate(TCollectionProxyInfo::MapInsert< map<string,int> >()));

      instance.AdoptAlternate(::ROOT::AddClassAlternate("map<string,int>","std::map<std::__cxx11::basic_string<char, std::char_traits<char>, std::allocator<char> >, int, std::less<std::__cxx11::basic_string<char, std::char_traits<char>, std::allocator<char> > >, std::allocator<std::pair<std::__cxx11::basic_string<char, std::char_traits<char>, std::allocator<char> > const, int> > >"));
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
         instance("map<string,bool>", -2, "map", 100,
                  typeid(map<string,bool>), ::ROOT::Internal::DefineBehavior(ptr, ptr),
                  &maplEstringcOboolgR_Dictionary, isa_proxy, 0,
                  sizeof(map<string,bool>) );
      instance.SetNew(&new_maplEstringcOboolgR);
      instance.SetNewArray(&newArray_maplEstringcOboolgR);
      instance.SetDelete(&delete_maplEstringcOboolgR);
      instance.SetDeleteArray(&deleteArray_maplEstringcOboolgR);
      instance.SetDestructor(&destruct_maplEstringcOboolgR);
      instance.AdoptCollectionProxyInfo(TCollectionProxyInfo::Generate(TCollectionProxyInfo::MapInsert< map<string,bool> >()));

      instance.AdoptAlternate(::ROOT::AddClassAlternate("map<string,bool>","std::map<std::__cxx11::basic_string<char, std::char_traits<char>, std::allocator<char> >, bool, std::less<std::__cxx11::basic_string<char, std::char_traits<char>, std::allocator<char> > >, std::allocator<std::pair<std::__cxx11::basic_string<char, std::char_traits<char>, std::allocator<char> > const, bool> > >"));
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
         instance("map<string,TChain*>", -2, "map", 100,
                  typeid(map<string,TChain*>), ::ROOT::Internal::DefineBehavior(ptr, ptr),
                  &maplEstringcOTChainmUgR_Dictionary, isa_proxy, 0,
                  sizeof(map<string,TChain*>) );
      instance.SetNew(&new_maplEstringcOTChainmUgR);
      instance.SetNewArray(&newArray_maplEstringcOTChainmUgR);
      instance.SetDelete(&delete_maplEstringcOTChainmUgR);
      instance.SetDeleteArray(&deleteArray_maplEstringcOTChainmUgR);
      instance.SetDestructor(&destruct_maplEstringcOTChainmUgR);
      instance.AdoptCollectionProxyInfo(TCollectionProxyInfo::Generate(TCollectionProxyInfo::MapInsert< map<string,TChain*> >()));

      instance.AdoptAlternate(::ROOT::AddClassAlternate("map<string,TChain*>","std::map<std::__cxx11::basic_string<char, std::char_traits<char>, std::allocator<char> >, TChain*, std::less<std::__cxx11::basic_string<char, std::char_traits<char>, std::allocator<char> > >, std::allocator<std::pair<std::__cxx11::basic_string<char, std::char_traits<char>, std::allocator<char> > const, TChain*> > >"));
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
         instance("map<string,Field>", -2, "map", 100,
                  typeid(map<string,Field>), ::ROOT::Internal::DefineBehavior(ptr, ptr),
                  &maplEstringcOFieldgR_Dictionary, isa_proxy, 0,
                  sizeof(map<string,Field>) );
      instance.SetNew(&new_maplEstringcOFieldgR);
      instance.SetNewArray(&newArray_maplEstringcOFieldgR);
      instance.SetDelete(&delete_maplEstringcOFieldgR);
      instance.SetDeleteArray(&deleteArray_maplEstringcOFieldgR);
      instance.SetDestructor(&destruct_maplEstringcOFieldgR);
      instance.AdoptCollectionProxyInfo(TCollectionProxyInfo::Generate(TCollectionProxyInfo::MapInsert< map<string,Field> >()));

      instance.AdoptAlternate(::ROOT::AddClassAlternate("map<string,Field>","std::map<std::__cxx11::basic_string<char, std::char_traits<char>, std::allocator<char> >, Field, std::less<std::__cxx11::basic_string<char, std::char_traits<char>, std::allocator<char> > >, std::allocator<std::pair<std::__cxx11::basic_string<char, std::char_traits<char>, std::allocator<char> > const, Field> > >"));
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
         instance("map<string,Field*>", -2, "map", 100,
                  typeid(map<string,Field*>), ::ROOT::Internal::DefineBehavior(ptr, ptr),
                  &maplEstringcOFieldmUgR_Dictionary, isa_proxy, 0,
                  sizeof(map<string,Field*>) );
      instance.SetNew(&new_maplEstringcOFieldmUgR);
      instance.SetNewArray(&newArray_maplEstringcOFieldmUgR);
      instance.SetDelete(&delete_maplEstringcOFieldmUgR);
      instance.SetDeleteArray(&deleteArray_maplEstringcOFieldmUgR);
      instance.SetDestructor(&destruct_maplEstringcOFieldmUgR);
      instance.AdoptCollectionProxyInfo(TCollectionProxyInfo::Generate(TCollectionProxyInfo::MapInsert< map<string,Field*> >()));

      instance.AdoptAlternate(::ROOT::AddClassAlternate("map<string,Field*>","std::map<std::__cxx11::basic_string<char, std::char_traits<char>, std::allocator<char> >, Field*, std::less<std::__cxx11::basic_string<char, std::char_traits<char>, std::allocator<char> > >, std::allocator<std::pair<std::__cxx11::basic_string<char, std::char_traits<char>, std::allocator<char> > const, Field*> > >"));
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
"/cvmfs/cms.cern.ch/el9_amd64_gcc12/lcg/root/6.30.09-a471e16c07e14e11bdeeaf6baa56e34c//include",
"/cvmfs/cms.cern.ch/el9_amd64_gcc12/lcg/root/6.30.09-a471e16c07e14e11bdeeaf6baa56e34c/include/",
"/u/user/jhlee/ttHH/CMSSW_14_2_1/src/tempTTHH/",
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
"eventBuffer::Electron_s", payloadCode, "@",
"eventBuffer::FatJet_s", payloadCode, "@",
"eventBuffer::GenJet_s", payloadCode, "@",
"eventBuffer::GenPart_s", payloadCode, "@",
"eventBuffer::Jet_s", payloadCode, "@",
"eventBuffer::Muon_s", payloadCode, "@",
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
