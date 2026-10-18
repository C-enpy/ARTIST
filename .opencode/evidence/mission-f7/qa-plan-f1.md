# F1 — plan QA validé avant auteur (2026-10-18)

Proposition lecture seule : f1-qa-plan ; persistance/runner lead distinct.
Contrat minimal : uniquement le type stocké des diagnostics AttributeContext.
Base code F2 `1690bac` (documents ultérieurs sans delta de ce header).
G1 candidat HEAD/delta/hashes à confirmer avant auteur, sinon réancrer.
ADR0003/5 Accepted partial ; 0004/6 Proposed ; 0008 Windows Debug seulement.

## Obligations

O1 GET et O2 SET : champ type stocké décrit std::shared_ptr<int> réellement dans
std::any, pas std::any lui-même. Noms calculés avec typeid dans le même binaire,
jamais chaîne MSVC littérale. O3 exception TraceableException<runtime_error>,
motif `ERROR::ATTRIBUTE::SET::TYPE_MISMATCH` inchangés (SET-in-GET conservé).
O4 freeze de type/payload préservés après rejet.

Inférées : O5 champs1 GET=typeid(T), SET=typeid(value) intouchés (asymétrie
existante non nettoyée ici). O6 empty→nullptr non changé, GAP accepté sans test
nouveau pour cette correction. O7 SET mismatch int→float atteignable, contrairement
à la déclaration historique du manifeste T4 (déjà rectifiée documentairement).

## Tests et oracles avant production

Fichier dédié recommandé accepté :
`ARTIST/tests/src/graphic/context/AttributeContextTests.cpp`.
Pas de mock/GL, API factice minimale si le template l'exige.

T1 GetValue_MismatchReportsTheRealStoredType : set shared_ptr<int>(42), puis
getValue<float> lève. Attendu construit via std::format avec motif historique,
champ1 typeid(float).name(), champ2 typeid(shared_ptr<int>).name(). Vérifier
getValue<int> retourne encore42 après le rejet.

T2 SetValue_MismatchReportsTheRealStoredType : set int42 puis set float1.5,
exception/motif préservés ; champ1 typeid(shared_ptr<float>).name(), champ2
typeid(shared_ptr<int>).name(). Après rejet, valeur int42 toujours accessible,
re-set int43 accepté et relu43. Ne pas remplacer champ1 par typeid(*value).

TestUtils expectSpecificError vérifie un contains ; le message complet reconstruit
est discriminant pour le champ2 dans le même binaire. RED doit montrer ce seul
écart de diagnostic, pas un changement de sémantique ou un problème de toolchain.

## Protocole et registre

Baseline CPU à recompter :249 attendus. Ajout test et reconfiguration GLOB_RECURSE,
pas édition CMake ; découverte251 attendue. RED ciblé2/2 échecs avant production,
logs et JUnit conservés. GREEN patch strict de deux expressions dans
`ARTIST/include/graphic/context/AttributeContext.hpp` :
`typeid(m_value).name()` → `m_value.type().name()` aux deux diagnostics.
Rien d'autre (pas de cast/refactoring/changement d'erreur public).
GREEN ciblé2/2, suiteCPU251/251 attendue, zéro failure/disabled/skip, découverte
et noms recoupés. Les nombres sont attentes, jamais résultats avant exécution.
GL/ASan non requis pour cette frontière diagnostique CPU ; aucun nouveau claimGPU.
Preuves `out/mission-f7/f1/` : commandes/logs/JUnit65536, hashes header/test,
G1/G2 et identité candidat testé/commité. Note attribution auteur/exécutant réels.

Registre : deux tests couvrent O1–O5/O7, O6 GAP accepté ; suite Attribute existante
non modifiée. Absence de doublon inférée par lecture ciblée, pas audit exhaustif.

## Risques, budget, décisions lead

I×P : portabilité diagnostic2×4=8 (typeid calculés), régression3×2=6 (queues
freeze/payload et suiteCPU), oracle non discriminant2×3=6 (RED exigé). Aucun>=10.
Budget20 : setup5, planifié11 (~73% du reste), buffer4 (~27%) pour glob/message
historique et réexécution. Surprise au-delà du buffer : retour au lead.

Décisions : fichier dédié, exactement2tests ; empty→nullptr gap accepté ; queue
re-set int obligatoire ; pas de nouvelle campagneGL. PM/LLM local développement
complexité brute35/retenue70 (plancher), risque30 indépendant. Un seul auteur
C++ pour tests+RED+patch+GREEN, isolé, build/cache sous bail, un job ; source et
sortie courtes explicites si C1083. Pas de Reader/Pass/Attribute wrapper/F2/T1-1,
mock/CMake/ADR. Plan commité avant worktree ; intégration lead puis revue fresh
bornée du diff et des preuves. Aucun PASS global.
