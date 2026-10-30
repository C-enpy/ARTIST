# Mapping proposé avant GREEN — garde CTest hardware

## Entrée et état

RED communiqué par le lead sur `19085a9ba1be1909be949eb74c96408930b6246e` :
20 méthodes, 33 failures de sous-cas, zéro error/skip, code1. Logs lead :
`out/verification-synthetic/context-collector-red-2026-10-30T04-13-37-465Z/`.
Cause observée : refus inconditionnel hardware CTest/code3 avant le nominal.
La sensibilité individuelle des vérifications aval n'est donc pas encore prouvée.

Contrôles auteur : parsing AST sans import/exécution du module, syntaxe valide,
trois méthodes recensées ; `git diff --check` code0 ; diff contre `19085a9` vide
pour la production, les 20 tests collecteur et le fichier de régression acquis.
Ces contrôles statiques ne constituent pas le RED.

Cette remise est le mapping demandé avant modification des anciennes attentes.
Aucun changement production ou test acquis dans ce commit. Trois méthodes de
garde supplémentaires sont préparées dans un nouveau fichier possédé, sans GREEN
anticipé et sans test exécuté par l'auteur. Les 20 nouvelles méthodes précédentes
et les 144 acquises restent intactes.

## Unique attente acquise devenue trop forte

`test_review_regressions.py`, méthode
`test_known_unsupported_hardware_ctest_blocks_before_children_or_device_observations`
(lignes184–194 sur cette base).

Aujourd'hui, le faux adaptateur historique ne déclare aucune des six variables
de contexte. Le cas doit **continuer de donner BLOCKED/code3**, avant tout enfant,
sonde de révision, parseur workload ou observation de device. Le test conserve
son nom et ces assertions, en particulier l'absence d'appel au runner.

La seule assertion obsolète est `fixture.adapter.calls == adapter_calls` : le
nouveau contrat exige d'inspecter les six valeurs explicites de chaque invocation
JUnit. Elles ne sont disponibles qu'en appelant une fois le service pur
`describe_execution(plan, build)` ; le double historique journalise cet appel par
`'run'`. L'attente proposée devient exactement :

```python
self.assertEqual(fixture.adapter.calls, [*adapter_calls, 'run'])
```

Ce n'est pas une autorisation d'appeler `collect_environment`, un constructeur à
effets, une sonde GPU ou un enfant. Les autres assertions de cette méthode restent
identiques, y compris le compteur de révision. La description doit donc précéder
la nouvelle observation de source/build, après validation du predecessor déjà
acquise. Aucun retrait de test, skip, nombre historique modifié ou assertion
favorable substituée.

La recherche exacte ciblée des références de garde dans tous les `test_*.py` de
`verification_core` n'a identifié aucune autre assertion acquise à réviser. Les
tests sans cible déclarée continuent de refuser avant **tout** appel adaptateur,
y compris cette description. Les sentinelles list/plan et descriptions pures
restent inchangées. Cette recherche textuelle n'est pas une preuve d'exécution.

## Condition proposée, privée et sans état de capacité

1. Valider le predecessor et la cible matérielle, sans changement d'interface.
2. Décrire une fois les commandes par l'adaptateur explicitement composé ; cette
   description est pure. Ne pas appeler son collecteur d'environnement.
3. Avant toute commande, inspecter chaque `--output-junit` de la sélection
   hardware : activation, run_id, hash plan, request_id et paire vendor/renderer
   doivent être explicitement présents dans `env_delta`. Aucun environnement
   hérité ou contexte préflight ne peut les remplacer.
4. Absence/transport partiel : refus préalable BLOCKED/code3, bruts de commandes
   natives inexistants puisqu'aucun enfant n'est parti. Valeurs complètes mais
   étrangères/contradictoires ou request_id dupliqué : panne de protocole/code4
   préalable. Aucune capability publique, cache de disponibilité ou nouveau
   champ persisté/public ajouté.
5. Pour les commandes admissibles seulement, continuer le contrôle d'identité,
   l'exécution et la collecte existants. Conserver un mapping privé chemin JUnit
   → identité de son invocation ; le nom réel du cas fournit la septième clé.

La CLI ne peut plus classer toute sélection historique comme unsupported à partir
du seul plan : elle doit composer l'adaptateur fourni puis laisser ce contrôle
pur des commandes au cœur. Sa garde **cible absente** reste antérieure au factory.
Le nominal CLI utilise exclusivement le factory factice injecté, sans import de
modules adaptateurs de production encore non intégrés. Leur future composition
réelle reste un gap d'intégration et n'est pas un RED d'import à fabriquer.

## Compléments nouveaux préparés pour RED lead

Nouveau fichier `test_context_preflight_contract.py`, trois méthodes et variations
subTest pour les contre-exemples de garde :

- aucune des six variables, puis chaque omission individuelle : BLOCKED/3,
  zéro enfant, zéro observation device, zéro nouvelle sonde de révision ; seule
  une description pure est autorisée ;
- paire/identité complète étrangère et request_id réutilisé entre invocations :
  protocole4 avant tout enfant, sans fallback ;

La garde cible absente antérieure à toute description/factory reste couverte par
les tests acquis, sans duplication ou modification ici.

Ces contrôles sont des nouvelles assertions de sécurité, pas la modification des
20 méthodes déjà rouges. Le lead doit observer leur RED causal avant leur GREEN ;
le premier lot de 33 failures ne qualifie pas leur sensibilité. Aucun test ou
processus par l'auteur. Les éventuelles limites d'accès/ownership doivent être
résolues par le lead, sans remplacement des assertions acquises.

Commande ciblée proposée au lead après revue du mapping :

```text
C:/Python313/python.exe -B -m unittest discover -s scripts/tests/verification_core -p 'test_context_preflight_contract.py' -v
```

Attendu RED : absence/omission donne déjà3 mais ne passe pas par la description
pure prévue ; les transports étrangers/dupliqués donnent actuellement3 au lieu
de4. Aucun ImportError ou effet d'outil réel n'est attendu. Les compteurs figent
zéro enfant/sonde/device, pas seulement le code de sortie.

## Collecte et rapport prévus, sans changement d'oracle

Réemploi du parser intégré pour une et une seule observation par cas OpenGL
hardware, jamais exigée sur unit.*. Réemploi du gardien historique pour les
populations, en séparant sa réconciliation des skips ordinaires de la précondition
structurée admise. Conserver XML, sorties, retours natifs et population avant
qualification. Collecter toutes les causes du lot, sans dépendance à l'ordre des
cas : précondition+skip3 ; assertion5 ; protocole4 ; interruption130. Une assertion
reste FAIL même si un autre cas est indisponible ou malformé. Le rapport ne sonde,
n'écrit, ne relance et n'invente rien ; contrôle des identités/bruts inchangé.

Ownership inchangé : sink possède les fichiers ; le parser reçoit le system-out
local, aucun cache ou emprunt GPU. Runner, modèle, adaptateurs, C++ et schémas hors
modification. Un seul écrivain par bundle et source stable pendant la phase.

## Frontière à résoudre par le lead

Le dernier message autorise `evidence.py`/`reporting.py`/CLI conditionnel et des
**nouveaux** fichiers tests/docs. L'unique assertion acquise ci-dessus est dans
un fichier existant, donc elle n'est pas éditée implicitement. Demande bornée :
valider ce mapping et transférer l'ownership de cette seule assertion, ou fournir
la correction intégrée équivalente. Les autres assertions du fichier ne changent
pas. Les nouveaux contre-exemples sont RED_READY pour le lead avant la correction
minimale, sans extension de mandat ni nouvelle permission d'exécution.
