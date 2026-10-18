# Résultat — ingestion documentaire réduite

2026-10-18 ; auteur et runner : lead. Périmètre confirmé par l'utilisateur après
demande de simplifier, puis « go ». Aucun nouveau worker de rédaction/revue.
P0 lu, repris en checklist et accepté via candidate_id ; pool-1 arrêté.

## Candidat et livrables

Moteur historique : f7bb171e1c8ca10929467a0fcd50469dbfa69837.
Sources/checklist intégrées avant rédaction : d063ec9b618144526d758825d5829f016c52ccd0.
Une note `docs/references/session-2026-10-18-attribute-gl.md`, un backlink dans
la note Q3 propre et une entrée de manifeste local ignoré (non commité) ; règles
opérationnelles inchangées.
Source dérivée : source-ledger.md adjacent, SHA256
bb7893200110447200de620b0b4945734f6441453999a0bbc03fa8f4dde183d3,11926 octets.
Note finale SHA256 :
0bab1cc81f15ca71e57cf155074ca925a11b34b2baf78f145c1501f5efae78f6.

## Checklist → preuves (et réciproquement)

- E1 confidentialité : dérivation manuelle depuis texte visible, lecture du
  contenu final et contrôle borné de motifs credentials/clé privée/état provider ;
  aucun export privé dans docs. Ce n'est pas une garantie universelle de détection.
- E2/I1/I2 couverture :14 exports privés hash/size/comptes concordants ;13 sessions
  historiques,850 entrées, borne inclusive ;18 identifiants cités tous dans cette
  borne. Compaction, PM exclu et unités de comptage distingués ; pas de verbatim.
- E3/E4 qualification : chaque enseignement attribué ;326 résultats historiques
  recoupés avec XML/logs/manifeste, pas relancés ; ancien FAIL driver préservé ;
  clôture globale ouverte et statuts ADR conservés.
- E5 garde-fous :2 rappels candidats, explicitement non adoptés. Aucun profil,
  permission, moteur, script ou réglage de routage modifié dans cette ingestion.
- E6/E9 ownership :16 chemins dirty protégés inchangés, digest agrégé avant/après
  a92ae2467b7a009cc499622f460f0bec83526dfe3f3a4689ce17dc89ba1199f3.
  Index/log dirty non édités ; backlink ajouté dans une note propre à la place.
- E7 métadonnées : YAML parsé avec le parseur installé, summary141 caractères,
  4 wikilinks résolus,1 backlink ; manifeste hash/size vérifiés,318 sources et
  40 notes présentes. Ancien compteur pages37 était déjà en retard avant ajout.
- E8 retrouvabilité : `node .opencode/scripts/update-wiki.mjs` → artist-wiki seul,
  1indexé,1misàjour,38inchangés,0retiré/skip. `qmd embed -c artist-wiki` →
  7chunks/2documents,14s. Query lex850 dans artist-wiki avec intent → nouvelle note,
  puis get lignes119–150 confirme propositions/non-adoption et clôture ouverte.
  La première recherche de phrase composée ne retournait rien ; le terme distinctif
  a permis de vérifier la retrouvabilité, sans changer de collection ni d'index.

Tous ces contrôles ont été exécutés par le lead via Context Mode ; aucune preuve
de reviewer n'est réattribuée au lead ou inversement. Git diff --check exécuté sur
les modifications suivies ; nouveau Markdown relu et parsé avant indexation.
Une commande git add a constaté que .opencode/evidence était ignoré : fichiers
déplacés sous la mission puis commit ciblé, sans forcer l'ajout ni modifier ignore.
Le manifeste docs/.manifest.json est également ignoré : maintenance locale
conservée, explicitement exclue du commit plutôt que publication forcée.

## Limites

Pas de nouveau test moteur/GPU, image, performance, ASan, Release ou autre OS/API.
Pas de nouvel audit indépendant final dans ce périmètre réduit ; le plan P0 n'en
tient pas lieu, ni la revue Q3 historique. Aucun PASS global QA/moteur revendiqué.
Intégrité documentaire et retrouvabilité contrôlées seulement. Les gaps de clôture
moteur historiques restent hors ingestion. Aucun contrôle inventé pour faire RED.
