# B3 — Proposition de contrat du Reader, non adoptée

2026-10-25. Auteur b3-contract/engine-architect, remise native complète ; persistance lead. Lecture seule, aucun essai/édition/build par auteur. Début/fin HEADb7cc884 lu, source82bf52d attribuée au brief. Serena a fourni les corps ; références template readShaderFile={} non probantes, grep exact complémentaire. QMD absent du profil : digestion fournie par lead, refus externe non répété.

## Chaîne observée et défaut

Shader.hpp IShader::load96–107 lit si code vide ; Shader::read199–204 sélectionne Reader par if constexpr ; OpenGLShaderReader<Classic>::on appelle detail::readShaderFile124–126. Celui-ci vide code67, possède ifstream68, ouvre78, extrait89, clear/close95–99, publie103 ; catch105–110. extractShaderSource30–51 accumule par read/gcount4096, rejette seulement fail&&!eof. ShaderContext possède path/code ; Loader crée/compile un objet GL seulement après retour normal.

Sonde réelle lead ancienne25 : baseline/recovery4608octets ; sous LockFileEx, ReadFile échoue33 mais ifstream bad0/eof1/fail1/gcount0 sans exception et Reader publie vide. Flags seuls, rejet de tout fail, déplacer clear ou sizecheck ne rétablissent pas l'information perdue. Aucun dispositif autorisé n'a démontré un ifstream::failure authentique ni close réellement échoué.

## Contrat proposé

Chargement source synchrone, API publique on(Context&) et Shader/Loader inchangés. Contexte vivant, accès sérialisé, source stable pendant appel ; pas de snapshot/version cohérente garantie si mutation concurrente. Pas de nouveau contrat UTF8/wide/sandbox.

Option recommandée **à calibrer d'abord**, pas autorisation de code : fopen/fread(size1)/ferror/feof/fclose de la CRT existante, aucune nouvelle dépendance/API/seam ni couche de politiques générique. Si cette CRT perd l'erreur comme ifstream, rejeter l'option, pas une heuristique.

1. Code vidé au départ, owner local unique du flux, chemin emprunté synchroniquement. Ouverture impossible : rejet/code vide.
2. Accumulateur local ; après fread, capturer n/flags et errno indicatif avant cleanup. ferror prioritaire même avec feof ; rejet sans préfixe publié. Sinon ajouter exactement n ; EOF termine normalement y compris court/zéro. Zéro sans EOF/erreur : absence de progrès rejetée.
3. Lecture réussie **et fclose réussi** avant publication unique. Fermeture une fois aussi sur erreur/allocation, garde non jetante. fclose consomme le flux même en échec : aucun retry/double-close. Préserver erreur primaire si close échoue aussi, ne pas masquer bad_alloc ni changer les catégories non-I/O.
4. Publication finale unique, vide légitime inclus ; rejet laisse code vide, ni ancien ni partiel. Garantie pour setter concret actuel, pas un override arbitraire qui modifie puis jette.
5. Diagnostics proposés : TraceableException<runtime_error>, ouverture/lecture : ERROR::SHADER::FILE_NOT_SUCCESSFULLY_READ puis newline Reading stopped without reaching end-of-file. ; close : même préfixe puis newline Closing the file failed. Pas de suffixe errno variable public. Retrait du e.what() du catch historique à décider explicitement.

## Compatibilité et binding times

Conserver texte natif antérieur, pas passer binaire silencieusement : comparer CRLF/Ctrl-Z/BOM/NUL avant fixation modeCRT. ASCII/LF : exact pour vide/court/multiples4096. Caractériser chemins espaces/nonASCII dans encodage antérieur std::string. Préservation NUL dans string ne prouve pas support GLSL binaire (Loader utilise c_str/longueur nullptr, hors B3).

Owner FILE* non copiable ; aucun handle publié. Emprunts path/code invalidés par mutation/move/destruction, aucun accès concurrent. Aucun GLuint modifié par Reader, aucune retraiteGPU traitée par closefichier. Compilation : composant Classic/types/adaptateurOS éventuel borné ; link : header-only/CRT existante ; cook ne résout pas erreur fichier vivante ; lecture/EOF/close/publication restent runtime par fichier/bloc4096, pas par assettype ni draw/frame. OpenGL seul backend, pas de spécialistes autres API requis.

## Alternatives et migration

- Garder ifstream : plus simple, mais informations insuffisantes observées ; ne constitue pas correction. Changer masque n'est pas preuve authentique.
- Stdio : recommandé sous calibration erreur et modes, surface faible, mais remplacement de catch à adopter.
- Win32 ReadFile/CloseHandle : erreur33 directe ; plus de mapping texte/encodage/partage/cleanup, alternative si stdio échoue, pas fallback implicite ni portPOSIX demandé.

SourceExtractor/tests scripted existants : ne conserver que les sous-contrats réellement utilisés, sinon suppression/remplacement explicitement adopté ; pas de code mort laissé pour annoncer catch couvert. Plan de migration des oracles requis avant auteur, aucune baisse de garantie publique.

## Matrice de preuve proposée, non exécutée

- M0 nominal : fichiers possédés0/4096/8192/3*4096+37 et4608 connu ; octets exacts et fermeture avant publication, modes/chemins comparés. Pas campagneGPU.
- M1 extraction authentique : ouverture réussie puis verrou recouvrant lecture, témoin ReadFile33, stdio attendu ferror!=0 (même si feof), Reader rejette/code prérempli vidé, aucune publication partielle. Capturer opérations/codes/nombres sans modifier flags/masque/rdbuf/throw. Si load complet exercé, aucune compilationGL après rejet via moyens existants.
- M2 recovery : unlock puis lecture exacte/publication après close, cleanup possédé séparé et voisins préservés.
- M3 erreur après préfixe : hypothèse supplémentaire après M1 ; verrou sur plage ultérieure, calibrer n>0 réellement acquis avant erreur sans supposer préfetchCRT. Sinon gap, pas sweep.
- M4 catch authentique : verrou actuel n'y entre pas, aucun dispositif autorisé identifié. Stdio remplacerait cette branche par erreurI/O authentique : **substitution d'obligation explicitement acceptée**, jamais ferror dit catch exercé ; sinon obligation ouverte.
- M5 close réellement échoué : aucun dispositif déterministe identifié avec fichier régulier local lecture seule/owner valide/ressources actuelles. Lock/rename/suppression/doubleclose/handle invalidé artificiellement ne prouvent pas ce négatif. Ni impossibilité universelle ni inatteignabilité prouvées. Autoriser rechercheOS ciblée distincte ou disposition résiduelle motivée acceptée ; pas DEFER automatique/PASSbranche.

Calibration nouvelle : dériver commandeCL réelle de la base de compilation/protocole25 et wrapper existant, runner lead seul. Toute sonde exécutable est une tranche de développement évaluée≥70 avec contrat/QAavant auteur ; pas de shell/compiler inventé par architecte. Ancien probe sans fread ne doit pas être relancé à l'identique. Après adoption/QAcommité seulement : RED sur Reader actuel au verrou, correction minimale/GREEN même opération/recovery puis sélection finale proportionnée, identités/logs complets. Aucun transfert automatique ASan555 sur nouveau diff.

## Arbitrages encore ouverts

Calibrer ou non stdio ; mode/chemins/messages et retrait éventuel catch/seam ; recherche authentique close ou disposition résiduelle. Contrat proposé seulement, aucune ADR créée/adoptée ni F8 clos. ADR0001/2/4/6/7Proposed,0003/5partial,0008WindowsDebug seulement.
