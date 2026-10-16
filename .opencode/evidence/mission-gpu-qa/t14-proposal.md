# T14 — generator de gaps après intégration

Date : 2026-10-16. Proposition generator-proposal, persistée par le lead.
PM/LLM brut C66/R56 → retenu C70/R56 ; évaluation manuelle inchangée.
Profil réellement admis : **build-engineer** ; méthode : **test-engineer** lue en
entier (229 lignes), substitution explicite, pas activation du profil QA natif.
Input fourni : `73feaf52b519d3e67bf98524d6a1242f4072c21c`, branche isolée
`mission/gpu-qa-integration-e52f73a`. Identité fournie par les preuves intégrées,
pas un nouveau Git/hash exécuté par ce worker. Voir qa-admission-limit.md.

## Actions réellement effectuées

Session déplacée nativement avant enquête vers le vrai worktree. Lectures :
AGENTS/principes, plan QA, frontière de policy, rapport de validation, preuve
d'intégration T13, limite d'admission, catalogue et union ; T13 test entier,
T04:501–669, T05 tests/oracle, T03 §§5–8 et plan de probes natifs.
Autres producteurs/provenance non réaudités exhaustivement.
Aucune édition, commande shell/Node/Git/réseau, test, canari, MCP exécuteur,
grant ou sous-agent. Navigation/board/restitution du substitut ne prouvent
aucune capacité des trois profils QA. Pin historique du profil non réécrit.

**Zéro proposition de test, zéro patch, zéro réparation healer, zéro test exécuté
par T14.** Aucun nouveau RED/GREEN ni PASS d'exécution T14.
Runs antérieurs attribués au lead : 355 T04 + 170 T05–T12 + 63 T13 = **588**,
zéro échec/skip/cancel ; union syntax/schema/offline 41/81/215 PASS locaux et
missionVerdict INCONCLUSIVE. Source de ces nombres : t13-integration.md, pas
une exécution nouvelle du generator.

## Registre obligation → scénario → oracle → décision

G-01…07 et T-01…06 explicites ; G-08/T-07 inférés puis adoptés pour le harness.
Aucune présence statique promue en couverture native.

### KEEP — G-01/02/03 : inventaire local

Risque d'association/ID/lien omis ou collision. Nominal : 19 responsabilités,
quatre expertises, OpenGL seul backend, union 23 profils/17 skills ; négatif :
association incorrecte, doublon, skill/lien absent. Oracle indépendant T03 et
catalogue manuel. T13:56–107,154–172 et mutations CLI:189–251 contrôlent la
déclaration, les minima et les codes causaux. Les longueurs minimales de texte
ne prouvent ni justesse technique ni licences, et ce n'est pas un routeur.

### KEEP — T-02 / I2 / I6 / I7 : boucle et scores déclarés

Risque de confusion PM/QA ou barrières altérées. Trois triggers OR, seuil PM50
distinct de QA I/P1–5 et seuil10, plan commité avant auteurs, intégration avant
generator, reviewer fresh hors board/ré-audit et runner distinct. Négatif local :
valeur/étape modifiée ou clause retirée. Oracle plan/T03 ; T13:67–73,109–143 et
orchestrator:154–189. KEEP statique, pas exécution des gates du coordinateur.

### KEEP — T-01/03/04/05 et T-06 déclaré

Portable avant pin, generator complémentaire au TDD, healer deux réparations
maximum, runner séparé/template exact. Négatifs : ordre/clause/garde/identité ou
autorité candidat altérés. T05:49–101/expectations.json et T04:501–624 couvrent
ces discriminants. Une nouvelle copie des 27 règles serait redondante ; aucun
nouveau cas établi pour rouvrir F1–F6. Forme B = static-presence, pas enforcement.

### KEEP — G-06/G-08/T-07 : honnêteté locale

Risque de manque de preuve/candidat périmé blanchi. Nominal non nul et identité
liée au candidat ; négatifs zéro/skip/stale, entrée native vide + autre FAIL.
T13:197–198,232–250 et T04:610–624 distinguent exit2/exit1 sans PASS mission.
Autres négatifs et répétition offline byte-identique attestés par les rapports
lead, non rejoués ici. Interruption/publication native non couverte.

### REJECT comme ajouts ou preuves

Lookup JavaScript simulant sélection/gate ; regex ou copie de policy présentée
comme enforcement ; mock GPU ou symptôme fictif fermant G-05/06. Le test
replaceAll(...).includes(...) de T13:132 est une propriété de chaîne, pas une
preuve supplémentaire de workflow ; le contrôle de présence précédent garde
son utilité limitée. Aucun changement de ce test demandé.

**MODIFY : aucun.** Aucun oracle/assertion affaibli ou correctif de production
proposé. Aucun finding haut sans scénario discriminant nouveau.

## DEFER proposés — ni GAP-accept ni décision humaine implicite

1. **T-06/G-07 : blocked.** Vrais QA absents du root d'admission, présents dans
   le worktree ; admission depuis celui-ci refusée. Ce n'est pas un RED ni un
   refus du gate QA. Propriétaire lead ; avant toute revendication d'autonomie/
   enforcement, contexte d'admission autorisé, vrais profils/règles/sentinelles.
   Aucun contournement copie main, grant, execute/team_* ou transport nouveau.
2. **G-03/G-04 : GAP-defer proposé.** Catalogues observés à 0727bbc = découverte,
   pas lancement/sélection/chargement ; S01–S12 et OP1–OP3 NOT_RUN. Lead sous bail,
   avant PASS G-04/global ; utiliser native-probe-plan, pas une table locale.
3. **T-02/03/04/05 : GAP-defer proposé au niveau natif.** Ce livrable applique la
   méthode generator, pas le comportement du test-engineer natif. Revue finale
   indépendante fresh hors board après intégration T14, fallback encore signalé.
   Gates de workflow à éprouver ; T14 ne remplace ni RED auteurs ni revue finale.
4. **G-05/06 et interruptions natives G-08/T-07 : DEFER au niveau requis.**
   Aucun GPU/image/perf ajouté pour Markdown ; lead puis spécialiste compétent
   si le critère est demandé, preuve du candidat avant revendication.

## Top trois risques QA, distincts du PM

- **I5×P4=20, permissions** : template/fallback pris pour gate ; portée
  écritures/commandes hors QA ; détection profils/refus/sentinelles ; mitigation
  deny-all inchangé et lead distinct ; gap admission/gates/transport natif.
- **I4×P4=16, routage** : catalogue pris pour sélection ; portée expertise ou
  chargement inadaptés ; détection traces principal/appels/références ; mitigation
  oracles/clarification ; gap 0/12 observations natives.
- **I5×P3=15, faux vert** : 588 tests/substitution assimilés à autonomie/GPU ;
  portée décision globale ; détection attribution/candidat/niveau/comptages ;
  mitigation INCONCLUSIVE/FAIL prioritaire/revue fresh ; gap natif/revue finale.

Sens inverse : T13 catalogue/domaines/union → G-01/02/03 ; qaLoop → T-02 ;
minima → G-04 déclaré ; mutations CLI → G-03/06/07. T05 → T-01/03/04/05 et
T-06 déclaré ; T04:501–669 → G-03/06/07/T-06 déclaré. Aucun nouvel orphelin
introduit ; pas d'attestation exhaustive sur tous les tests préexistants.

## Décision et attribution du lead

**KEEP l'existant, aucun code à appliquer/exécuter au titre T14.** Aucun gap local
matériel nouveau démontré dans le périmètre examiné ; les risques prioritaires
restants requièrent natif ou revue indépendante. Ajouter un test textuel ne les
fermerait pas. Cela ne certifie pas l'exhaustivité des 588 tests.
Auteur proposition : generator-proposal ; persistance/relecture : lead ; runner
distinct pour les runs précédents : lead. Zéro test nouveau n'est qualifié PASS.
Les DEFER restent des propositions sans acceptation humaine implicite ; mission
INCONCLUSIVE pour critères manquants. Revue finale fresh à libérer après ce commit.
