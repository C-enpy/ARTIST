# Attribute-GL — plan QA Q2 renforcé

## Attribution, intégration et autorité

Proposition qa-planner/pool-4, tâche task_mvdwt1px_000h_ffpre5, C61/R66 PM/LLM,
fresh, lecture seule. Synthèse intégrée par le lead du candidat natif
result_mvdxkbzg_000p_jx1oqi, session ses_eb0a06dffffe2UlQEIFPkqRfsB,
message msg_14f68878c001i1e54sCJzsLyJP. Le texte natif complet reste disponible.
Ce fichier remplace le plan Q1 actif, archivé dans git (4a6276a/ece666c).
Q2 n'a écrit ni exécuté de test. Le lead persiste/relit/commit avant worktree D.

Entrées : attribute-contract.md, attribute-oracles-a2.md intégrés ece666c,
pm2-plan.md intégré6b2488c, décisions utilisateur « Renforcer avant l'auteur ».
Producteurs A2 task_mvdvuv25_0009_lutmr8 et Q task_mvdqhtoe_001j_edgftw clos,
sans branches à merger. Moteur déclaré identique à4a9b722. Diff externe harness,
profils,AGENTS.md,.grepai/config.yaml préservé hors périmètre et hors commit.

Pin observé : profils datés2026-10-16/e4b0aa7, skill40a7cc3, candidat moteur plus
récent. Q2 n'avait ni shell ni execute et n'a pas revérifié les SHA par git.
Le lead recoupe l'identité réelle avant auteur. Schéma JSON version1 distinct
de l'approbation mission v2. Aucun changement des scores/grille/provider.

ADR0003/5 Accepted/partial ;0008 Accepted/verified Windows Debug seulement ;
0001/2/4/6/7 Proposed/not-started. Pas de nouveau backend,lifetime VBO,RAII globale,
ASan,perf,image,Release,autreOS/mesh ou deuxième lecture Reader imposée.

## État observé et preuves disponibles

Static-presence Q2 : Setter garde buffer0/null puis BufferData sur buffer courant
et Pointer avec GLSize/GLType ; Reader char[256], pas de MAX_LENGTH, conversion
location non signée, pas de flag array ; addAttribute remplace une clé.
HasComponent vérifie seulement un alias de type, Setter primaire a on : la branche
UNSUPPORTED_TYPE peut être inatteignable sur OpenGL. Contexte sans initialiseurs
GL ; setGLUsage possède déjà une garde. Loader n'a pas de crochet TF.
Mock IPointer absent, GetProgramiv écrit1 pour tout pname ; nom court injecté.
PassTest aPos ne vérifie que clé/type. Tests GL lient leur propre VBO avant set,
pas de leurre et entier jugé seulement GL_NO_ERROR/taille ; cleanup à renforcer.

Seule exécution de mission disponible : baseline lead anciens binaires40/40,
36unit/4opengl,0fail/skip/disabled, sans rebuild, du2026-10-18T13:59:32.405Z
au13:59:35.178Z ; HEAD4a6276a54fd3007f73dfcf16c240bf42789f54fd.
Commande complète/empreintes : baseline-existing-attribute.json.
JUnit out/attribute-gl/baseline-existing-attribute.xml,
SHA256 f792498eb3d3f1abc40c745276775078186cf66ad5dbe37f2b51841b6419d1b1.
Log homonyme .log. Les tests OpenGL ont été relancés via leur fixture réelle ;
aucune identité driver/capacité pour les nouveaux oracles n'a été relevée ici.
Ce PASS porte seulement sur les40 anciens tests, pas sur le contrat nouveau.
Historique299/299 et baselines87+22/109/122 restent périmés pour ce contrat.
Q2 :0découvert pour run,0exécuté,0échec,0skip ; aucune mesure/performance.

## Rectifications de Q1 et stratégie renforcée

Source primaire vérifiée par lead : GLSLangSpec3.30,révision6,11-Mar-2010,
§4.3.4,p29/PDF35 : les entrées vertex peuvent former des tableaux de ces types,
mais pas des structures. URI et copie locale dans attribute-oracles-a2.md.
L'échec attendu d'array[1] dans Q1 est FAUX ; sa sonde attend désormais compile/link
succès et relève la réflexion réelle. Refus arrays à l'utilisation reste le contrat.

Ancienne capture d'un out vec4 converti float retirée :12sorties identité typées,
interleaved120octets + separate4buffers. Pas de report des entiers hors float32.
DH2/DH3/DH4 de Q1 ne sont pas des dérogations acceptées. Les états impossibles
restent gardes du produit avec preuve CPU étiquetée et décision explicite ; pas
couverture native inventée. L'exhaustivité de tous les entiers n'est pas revendiquée.

## Classes exclusives de preuves

- COMPILE_TIME : layouts/is_same/présence ou absence de on.
- NATIVE_STATE : requêtes après set sur programme/VAO/driver réels.
- NATIVE_TF_INTERLEAVED et NATIVE_TF_SEPARATE : octets capturés typés sans cast.
- NATIVE_READER : réflexion réelle puis OpenGLPassAttributeReader.
- NATIVE_API_PROBE : appel GL direct hors Reader (exemple bufSize4).
- COMPILE_REFUSAL et LINK_REFUSAL : shader/programme refusés, Reader non appelé.
- CPU_FAULT_INJECTION : métadonnées injectées, symbole contenant FaultInjection.

Un mock/interpose n'est jamais une sortie GPU. Absence d'observation native =>
GAP/blocked/INCONCLUSIVE, pas COVERED par analogie ou par zéro GL_NO_ERROR.

## Obligations explicites E1–E23 et intent

E1 : float/vec2/3/4,int32/ivec2/3/4,uint32/uvec2/3/4 canoniques packed seulement.
E2 : tailles/alignements4/4,8/4,12/4,16/4 ; offsets i*4,stride sizeof(T),is_same
des alias A. Aucun treizième type ou qualifier alternatif implicite.
E3 : hors famille set compile mais lève UNSUPPORTED_TYPE sans GL.
E4 : enum GLSL brut/cardinalité séparés ; type non supporté décrit sans rejeter Pass.
E5 : nom dimensionné MAX_LENGTH,NUL/length cohérents,pas clé tronquée.
E6 : INVALID_NAME,DUPLICATE_NAME,INVALID_LOCATION,INVALID_SIZE avant publication
de l'attribut invalide ; INVALID_PROGRAM_ID inchangé.
E7 : GLint location contrôlé avant conversion ;0valide,-1refusé.
E8 : setGLArray/isGLArray défautfalse,suffixe exact[0] même size1,pas size>1 seul.
E9 : buffer/id/type/size/usage0,arrayfalse ; id0n'est pas sentinelle invalide.
E10 : ordre Setter buffer0,valeur absente,array/size,enum,usage,puis GL.
E11 : au refus zéro BindBuffer,BufferData,Pointer,IPointer.
E12 : float Pointer(location,N,GL_FLOAT,GL_FALSE,sizeof(T),nullptr).
E13 : entiers IPointer avec GL_INT/GL_UNSIGNED_INT,jamais Pointer.
E14 : bind VBO propre avant upload ; GL_ARRAY_BUFFER reste cette cible au succès.
E15 : leurre taille/octets intacts après succès et refus.
E16 : payload stocké avant Setter reste au refus ; TYPE_MISMATCH/messages inchangés.
E17 : ownership unique/emprunts F7 inchangés,pas destructor/delete VBO production.
E18 : Binder/Unbinder et UNBIND::BUFFER_ID_NOT_SET inchangés.
E19 :12formes interrogées et consommées dans leur type sur vrai GL3.3core.
E20 : cleanup fixtures courant avant destroyWindow/terminate,même ASSERT fatal.
E21 : RED causal sur setup fonctionnel,GREEN minimal,refactor/nonrégression intégrée.
E22 : identité candidat,commandes,comptages/skips,classe/oracle/artefacts.
E23 : aPos/GL_FLOAT_VEC3/location0/size1/arrayfalse sur minimal.vert.

Intent : contrat interface shader/layoutCPU distinct,correction avant vitesse,
validation avant driver et isolation du VBO tiers ; aucun lifetime global ajouté.

## Inférences F — propositions locales, pas nouvelles obligations de production

F1 : primaire sans on,12spécialisations ; gate de IAttribute::set sur on réellement
emprunté (HasBorrowedOnMethod proposé),pas refonte globale de HasComponent partagé.
L'auteur vérifie le concept réel et préserve les autres backends/composants.
F2 : size/location testées sur GLint avant setters non signés.
F3 : doublon détecté Reader,addAttribute reste remplacement ; premier objet jamais
remplacé par le doublon rejeté,pas rollback de toute map imposé.
F4 : EnableVertexAttribArray dans fixture après set,jamais Setter ; mock zéro enable.
F5 : programme TF fixture,varyings avant link,Reader invoqué sur contexte du vrai
programme ; un owner du programme,pas hook TF ajouté à Loader/Pass public.
F6 :12VBO d'entrée/12set,120octets de capture (pas VBOmesh unique),pas struct C++
susceptible de padder vec3 pour comparer l'image.
F7 : mock GetProgramiv par pname,nom succès NUL dans bufSize,pname inattendu échoue.
F8 : usage0 initial testé sans setGLUsage ; garde existante de ce dernier conservée.
F9 : fixtures a0..a11,b0..b11,locations0..11 ; autres aPos/qaArray/qaArray2.
Pas enable production,backdoor usage illégal,second read obligatoire,flat ajouté
sans comprendre le log,ou sous-ensemble annoncé comme12.

## Préflight natif bloquant

Runner lead,GL3.3core ; une purge GLEW connue puis GL_NO_ERROR,pas purge des erreurs
scénario. Enregistrer vendor/renderer/version/GLSL/profil.
Minimums lus :MAX_VERTEX_ATTRIBS>=16,VERTEX_OUTPUT_COMPONENTS>=64,
TF_INTERLEAVED_COMPONENTS>=64,TF_SEPARATE_ATTRIBS>=4,TF_SEPARATE_COMPONENTS>=4.
Lire MAX_VARYING_COMPONENTS ; sortie planifiée30+gl_Position4=34composantes.
Une limite insuffisante pour son usage ou link12échoué => arrêt,sans retirer sortie.

Sondes sous test-datas/shaders/qa-attribute,refus en littéraux hors vertex/good :
- qaArray[1] utilisé : compile/link succès,nombre/nom/length/size/type/location réels.
- qaArray2[2],deux éléments actifs ; entrée active unique size2 valide à enregistrer.
- dvec3 sans extension,struct,bool : refus compile/link attendu,logs,Reader non appelé.
- vec3+mat4 légaux et utilisés : link attendu,puis Reader/mismatch.
- identifier300lettres puis256 si nécessaire ; succès=>oracle naturel de longueur.
- API directe bufSize4 sur nom plus long : troncature API,pas throw Reader production.

Preuve flag natif size1 seulement si qaArray[0]/size1/GL_FLOAT_VEC3,length exacte
et name[length]==NUL réellement observés. Nom nu/autre spelling ne sont pas réparés
pour coller à l'oracle : arrêter la prétention native et arbitrer.

## Mapping, payloads et captures exactes

Entrées/sorties de même type,identité sans cast. Offsets octets de capture :
- a0 float0.5 ->GL_FLOAT,N1,Pointer GL_FLOAT,@0.
- a1 vec2(16777216,16777218) ->GL_FLOAT_VEC2,N2,Pointer GL_FLOAT,@4.
- a2 vec3(-1,0.25,0.75) ->GL_FLOAT_VEC3,N3,Pointer GL_FLOAT,@12.
- a3 vec4(1,-2,0.5,16777216) ->GL_FLOAT_VEC4,N4,Pointer GL_FLOAT,@24.
- a4 int16777217 ->GL_INT,N1,IPointer GL_INT,@40.
- a5 ivec2(INT_MIN,INT_MAX) ->GL_INT_VEC2,N2,IPointer GL_INT,@44.
- a6 ivec3(-1,16777217,16777216) ->GL_INT_VEC3,N3,IPointer GL_INT,@52.
- a7 ivec4(INT_MIN,-16777217,16777217,INT_MAX) ->GL_INT_VEC4,N4,IPointer GL_INT,@64.
- a8 uintUINT_MAX ->GL_UNSIGNED_INT,N1,IPointer GL_UNSIGNED_INT,@80.
- a9 uvec2(0,16777217) ->GL_UNSIGNED_INT_VEC2,N2,IPointer GL_UNSIGNED_INT,@84.
- a10 uvec3(1,16777216,16777217) ->GL_UNSIGNED_INT_VEC3,N3,IPointer GL_UNSIGNED_INT,@92.
- a11 uvec4(UINT_MAX,16777217,2,2147483648u) ->GL_UNSIGNED_INT_VEC4,N4,
  IPointer GL_UNSIGNED_INT,@104. Fin120,vec3contigu12,pas paddingstd140.

Avant feedback,pour chaque index : binding après set=VBOpropre ; rebinding leurre
ne modifie pas ARRAY_BUFFER_BINDING capturé dans VAO ; taille/octetVBO=payload,
STRIDE=sizeof(T),SIZE=N,TYPE=scalaire,NORMALIZED=false float,INTEGER=true entier
et falsefloat,ENABLE après fixture,DIVISOR0 selon A2 ; leurre inchangé.
INTEGER/TYPE entier faux => échec immédiat,sans BeginTF ni draw indéfini.

Programme fragment minimal sans entrée entière,gl_Position=vec4(0)noncapturé.
Varyings b0..b11 avant link,RASTERIZER_DISCARD,POINTS(0,1),query PRIMITIVES_WRITTEN1.
Readback GetBufferSubData après EndTF ; pas glFinish requis. Comparer120octets
bitàbit à concaténation payloads,sentinelle0xA5 dès120 intacte.
Separate :second programme float,int,uint,vec4,valeurs a0/a4/a8/a3,4buffers,
GL_SEPARATE_ATTRIBS. Pas gl_SkipComponents/gl_NextBuffer/xfb plus récents.
2^24+1 et -16777217,INT_MAX,UINT_MAX discriminent pertefloat32. INT_MIN et
2147483648u sont eux représentables,mêmes bits possibles,donc TYPE distingue
signed/unsigned. Ne pas revendiquer exhaustivité de toute la plage d'entiers.

## Refus, état et RED causaux

CPU parseur foo[0]/size1 ->arraytrue,SetterARRAY_NOT_SUPPORTED ; suffixes foo[1],
foo[00],foo[0]bar nonmatch. Natif12clés/enums/locations,size1,arrayfalse,NUL cohérent.
Mat4 publié GL_FLOAT_MAT4/size1,Pass vivant ; set<vec4> mismatch expectedvec4/actualmat4,
leurre intact,zéroGL ; set<mat4> UNSUPPORTED_TYPE.
Chaque rang Setter combiné aux fautes ultérieures : buffer0 puisnull puisarray/size
puisenum puisusage0,premier diagnosticgagne,zéroBind/Data/Pointer/IPointer.
Conserver texte DRAWING_MODE_NOT_SUPPORTED et TYPE_MISMATCH existants.
Injections nommées FaultInjection :location-1,doublon,size0,countpositif/maxLength<=0,
lengthtropgrande,NULabsent/interne. ExceptionsE6,clé invalideabsente,aucun0xFFFFFFFF.
APIbufSize4 prouve seulement API. MêmeT secondset=>secondpayload ; autreT=>freeze,
premierpayload conservé,Setter nonrappelé.

RED anciencode attendu : leurreécrasé ; vec3enum/cardinalité GLSL transmisPointer
incorrects ; entierINTEGERfalse (sansTF) ; mockPointerau lieuIPointer une fois
symbole disponible ; nomlong tronqué si shader lié ; flagarray manque si[0]observé ;
injections sans exception. Floatscalaire mappingdéjàcorrect=>REDparleurre.
Mock manquant,include manquant,shaderillégal,contexteabsent/zérotest ne sont pas RED.
Setup mocks et sondes fonctionnels avant interprétation d'un échec.

## Registre bidirectionnel : obligation -> symbole, nominal et négatif

État nouveau partout GAP,ou blocked sur précondition indiquée. Aucun COVERED.
- O1/E1–3 AttributeContractTests :12layouts/is_same,négatifs famille/on ;
  SetterTests.UnsupportedType_ThrowsWithoutGL (aucunGL).
- O2/E9 SetterTests.InitialGlMetadata_IsDeterministic :objet sans value-init,
  tout0/arrayfalse ; détecte ancienindéterminé par initialiseurs/asserts.
- O3/E4,12,13 SetterTests.TwelveForms_BindUploadAndTypedPointer :12mappings,
  voieopposée0appels,ordrebind/upload/pointer.
- O4/E10,11 SetterTests.RefusalOrder_* :succèsO3,cinqrefus/combinations,zéroGL.
- O5/E14,15 leurre dans TwelveForms_NativeCapture :cible/VAOcorrects,leurreintact
  au succès/refus ; blocked sanscontexte.
- O6/E19 AttributeComponentTests.TwelveForms_NativeCapture :120octets,query1,
  sentinelle,refusdrawsiINTEGER/TYPEfaux ; blockedlimites/link12.
- O7 SeparateCapture_FourBuffers :4valeurs exactes,pasfloat32arrondi ; blockedlimites.
- O8/E8 AttributeReaderTests.SuffixParser_* :foo[0]vrai et suffixesnonmatches.
- O9/E8 ArrayInputs_RecordSpelling :exact[0]réelouarrêt ; blockedsonde.
- O10/E5 LongName_NativeReader :nomnaturelretenuexact,aucunmockfallback ; blockedlinknom.
- O11 ApiProbe_BufSize4 :troncatureAPIdirecte,pasthrowproduction ; GAPnativeprobe.
- O12/E6,7 AttributeReaderTests.FaultInjection_* :injections/diagnostics/cléabsente,
  GAPCPU,jamaisGPU.
- O13 MatrixPublishedThenSetterRefuses :publié/mismatch/leurre ; aprèslinklégal.
- O14/E23 PassTest.AttributeReadingTest renforcé :aPos/type/location0/size1/arrayfalse.
  Ancienpartielclé/type n'est pas preuve renforcée.
- O15 IllegalVertexInputs_DoNotLink :dvec3/struct/bool/logs ; linkréussi=>DH-REFUS.
- O16/E16 AttributeContextTests+Setter :secondpayload,messagesfreeze,autreTpasSetter.
- O17/E17,18,20 ownership/unbinderexistants+RAIIfixture :moved-fromFREED_CONTEXT,
  cleanupavantterminate mêmeASSERT ; ancienneownership n'est pas lifetimeGPU.
- O18/F4 requêteARRAY_ENABLED+mockzéroenableSetter :enablefixture seulement.
- O19/E21,22 registreRED/GREENrunner,candidat/hashes,compte>0/skips ; blockedrunner.

Sens inverse :toutnouveautest se rattache àO1–O19,sinonorphelinàrevoir.
Anciensexécutés40 :messagesGetValue/SetValue_MismatchReportsTheRealStoredType,
ownership/null/movedfrom,AttributeTestdispatchCPU,Binder/Unbinder,ReaderInvalidPassID,
contratsownership/formeon(C&),Passclé/type ; seulementanciencomportement.
Preuvesfaibles/périmées :SetIntUploadsOneIntWithoutGLError,SetVec3UploadsOneVec3WithoutGLError,
SetAttributeTest_CallVertexAttribPointer/WithVec3Stride,ReadAttributes_CallGetProgramiv/
CallGetActiveAttribAndGetAttribLocation. Ils ne prouvent ni IPointer ni noms longs,
ni leurre ni12formes ni matrice/tableau. Aucunnouvelorphelinexécuté,Q2n'écritpas.

## Registre risques : I/P distincts des scores PM

R-TF I5×P4=20 :sousensemble/GL_NO_ERROR/taille pris pour12captures,entiersPointer.
Déclencheurlink12échoué/limiteinsuffisante/ancienoracle ; blast radius3domaines et
verdictlot ; détection4anciensGLverts insuffisante ; mitigationcodeaucune ; gap
RED/limites/IPointerabsents. Prévuqueriesavantdraw et12TFexactes obligatoires.
R-ARR I4×P4=16 :flagdéclarénatifsans[0]ouancienrefuscompilationréutilisé.
Déclencheurspelling/repriseQ1 ; blastflag/parseur/fauxRED ; détectionspecvérifiée,
aucunesondedriver ; mitigationReaderaucune ; gapspellingnatifàobserver.
R-VBO I4×P4=16 :uploadbufferliépartiers ; déclencheurleurreintercalé ; blastVBO/VAO ;
détectionanciensfixtureslientbonVBO ; mitigationBinderseulement ; gapleurre/bindSetter.
R-GATE I4×P3=12 :horsfamilleatteintdriver viaHasComponent ; déclencheurpremiersetmat4/
dvec3 ; blasttype/étatGL ; détectionstatique ; mitigationbrancheUNSUPPORTED_TYPEmorte ;
gapREDruntimeetfermetureàétablir.
R-PNAME I3×P4=12 :MAX_LENGTHsurmockrenvoyant1/NULomis ; déclencheurReaderneuf ;
blastfauxGREENnom ; détectionON_CALLlu ; mitigationaucune ; gapmockparpname ensetup.
Tous>=10 :nominal/négatif automatisés ; R-PNAME traité avant interpréterlesnoms.
Buffer/hautrisque/TF jamais sacrifiés pour un itemmoyen. Réévalueràintégration.

## Budget retenu lead après recoupement PM2/Q2

Unités abstraites156=setup36+planifié90+buffer30. Setup23.1%total ; reste120,
planifié75%,buffer25%. Pas doublecompte. DifférencePM153 :+3bufferjustifiéparQ2.
Setup36 :limites/driver/purge6 ; sondeslégalité10 ; préflightnom300/256 6 ; mocks/RAII14.
Planifié90 :layouts4,exclus3,Reader8,oracleReadernom6,array8,mat4 4,Setter10,
erreurs8,leurre6,TFinterleaved14,separate8,injections6,nonrégression5.
Préflightnom=link,paslectureReader (postenomdistinct). Leurre/TFpartagentprogramme,
coûtsoracle distincts,pasdeuxlinksobligatoires. AucunposteTFcoupé.
Buffer30 :spelling/arbitrage6,aucunnomlong4,link12/varying6,purgeGLEW3,
GLM/mat4 3,NULdriver3,contexte/REDcausedifférente5.
Ordre :mocks/sondes,exclus/layouts,priorité/leurre,mapping,Reader,TFinterleaved,
separate,injections/nonrégression. PasASan/autreAPI/campagneentièrehorslot ajouté.

## Décisions humaines résiduelles, aucune GAP-accept implicite

**Décision utilisateur après intégration de Q2 : « Valider cette séparation ».**
Le lead a présenté explicitement les trois gardes Reader (location utilisateur
active négative, doublon d'attribut actif, longueur/NUL incohérents). L'utilisateur
autorise les injections CPU étiquetées pour ces gardes, complétées par les sondes
GL réelles, sans les compter comme preuve GPU de ces erreurs. DH-LOC/DH-DUP/
DH-TRUNC sont donc arbitrés pour ce lot : obligations de robustesse conservées,
oracles CPU_FAULT_INJECTION assumés et distincts des sorties driver.
Cela n'accepte ni l'ancien tableau mock-only de DH2, ni le report de captures
entières de DH4 :12 captures natives typées exactes/noms naturels restent requis.
Les autres arrêts conditionnels (spelling,nom,limites,contexte,alias/refus shader)
restent actifs. D est autorisable après commit de cette décision et du plan.

Utilisateur a refuséDH2–DH4Q1. Renforcement ne choisit pas spelling/noms/erreursnatives.
- DH-ARR-SPELL :arraycompile/linkoumetadataautrequeexact[0]/size1/type=>arrêt,
  observationpuisdécisionsuroracleoupreuveCPU ; pasnormalisationdriverimprovisée.
- DH-NAME :ni300ni256lié=>INCONCLUSIVE,déciderautrelongueurnaturelleouhorslot.
- DH-REFUS :dvec3/struct/boollié=>log,Readernonappelé,décision surpréconditiondriver.
  Arbitrage obtenu pour struct uniquement,après observation native sur NVIDIA617.42 :
  « Poursuivre, écart driver conservé ». Préflight de conformité struct resteFAIL,
  isolé et jamais transformé enPASS/skip. Interfacesstruct exclues des chemins
  Reader/Setter/draw/TF du lot ; bool exécuté indépendamment avant poursuite.
  Détails,noms/sélections et preuves :native-preflight-struct-incident.md.
  Toute sélection fonctionnelle est explicite et ne constitue pas unPASSglobal.
- DH-LOC :sanslocationnégativenaturellemêmebuilt-ins,INVALID_LOCATIONnatif
  inatteignable ; décisionexplicitesurpreuveCPUuniquedelagarde.
- DH-DUP :homonymes/location0activesrefusésavantReader ; décisionexplicitegarde
  DUPLICATE_NAMEuniquementCPU_FAULT_INJECTION,pasdriverfictif.
- DH-TRUNC :MAX_LENGTHconformeempêchetroncature ; bufSize4horsReaderprouveAPI,
  NULinterne/lengthincohérenteinjectés ; décisionsurthrowproductionnonatteignablenatif.
- DH-GLM :aliasdivergent,mediumpidentiquecanonique,vec3non12/4=>arrêt,pas13eforme.
- DH-LIMIT :deviceinsuffisant/link12échoué=>arrêt,pasdesousensemble ; device/lotàdécider.
- DH-CTX :contexteabsent=>aucunREDnatif,INCONCLUSIVEjusqu'àpréconditionfournie.

Ces règles définissent des arrêts conditionnels,pas des acceptations anticipées.
DH-LOC/DUP/TRUNC nécessitent clarification avant autorisation du consommateur D
si l'utilisateur exige une preuve GPU de chaque throw. Ne pas relire le refus
humain comme permission implicite d'enlever les gardes de production.

## Sortie, runner et ressources

Q2 sort avec proposition complète,persistée/committée parlead puisacceptationnative.
D nonpublié tantqueplancommité/décisionsbloquantesnonrésolues. Un auteur TDD,
uncompilerparsortie,unbailartist_opengl ; Q/generator/reviewer ne sontpasrunner.
AprèsD shutdown/merge/diff, G||R ; intégration avantT,préparationF||T,
verdictFaprèsnonnregression/candidatfinal,puisdocsL. Consumers restenthorsboard
jusqu'àentréesréelles. Productioncorrigée=>revues/preuvesaffectéesinvalidees.

Plus tard pour sortie lot :sondes/driver/logs/spellingexact ; tousDHrestantsnonacceptés
laissentcasesblocked ; REDcausauxpuisGREEN ;12image+4separate/sentinelle/query ;
nonrégressionTYPE_MISMATCH/INVALID_PROGRAM_ID/ownershipF7/Unbinder/aPos ;
comptagesdécouverts/exécutés/échecs/skips,candidat/delta/hashesbinaires/JUnit.
Zérotest/skiprequis=>INCONCLUSIVE ; ancien40/40nesatisfaitpasnouveaucontrat.

Commandes proposées, vérifiéesparlecture,pasexécutéesparQ2 :
`powershell -NoProfile -ExecutionPolicy Bypass -File scripts/build-windows.ps1 -TestLabel unit -Jobs 1`
Le script faitfresh/rebuild/ctestpreset ; compléter explicitement :
`ctest --test-dir out/build/windows-msvc-debug -N`
`ctest --test-dir out/build/windows-msvc-debug -L unit --output-on-failure --no-tests=error`
`ctest --test-dir out/build/windows-msvc-debug -L opengl --output-on-failure --no-tests=error --parallel 1`
Puis famille-RAttribute surbinairefraîchementcompilé,JUnitsousout/attribute-gl/.
Adapterracineauworktree,pasderéutilisationcacheautreracine. Conan2,CMakeDeps/
CMakeToolchain,Windowsx64Debug/MSVC14.44,MOCK_GLunitaireseulement,GLnatifdistinct.
Écritureautonome/enforcementQArestentnonprouvés,nouveautransportnatif≠sandbox.
