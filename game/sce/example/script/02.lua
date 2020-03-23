gstage = {
    title_id = "Stage2",
    turn_limit = 20,
    map = {
        size = {28, 20},
        terrain = {
            "WWWWWWWWffmmmmrrrrrrrrmmmmmm",
            "fffffffWffmmmmmrrrrrmmmmmfff",
            "fffffffWffffmmmmrrrmmmmmmfff",
            "fffffffffffffmmmmrmmmmmfffff",
            "fffffffWffffffmmmmmmmmffffff",
            "fffffffWfffffffmmmmmfffffmmm",
            "fffffffWfffffffmmmmffffmmmmm",
            "ffffWffWffffffffffffffmmmmrr",
            "ffffWffWffffffffffffffmmmrrr",
            "ffffWffffffffffmmffffffmmmrr",
            "ffffWffffffffffmmmmmffffmmmr",
            "ffffWffWfffffffmmmmmfffffmmm",
            "ffffWffWffffffmmmmmmmmfffffm",
            "fffffffWffffffmmrrmmmmmffffm",
            "fffffffWffffffmmrrmmmmmmfffm",
            "fffffffffffffffrrrrrfffffffm",
            "fffffffWfffffffrrrrrffffffmm",
            "fffffffWffffffrrrrrrffffffmm",
            "fffffffWffffffrrrrrfffffffmm",
            "WWWWWWWWfffffrrrrrrffffffmmm"
        },
        file = "map"
    },
    deploy = {
        unselectables = {
            { position = {9, 2}, hero = "CaoCao" },
            { position = {9, 1}, hero = "ZhangLiao" },
            { position = {9, 9}, hero = "XunYu" },
        },
        num_required_selectables = 0,
        selectables = {
            { position = {9, 4} },
            { position = {5, 3} }
        }
    },
    rewards = { -- NYI
        equipments = {
            { id = "short_sword", amount = 1 }
        },
        money = 1000
    }
}


function on_deploy(game)
    game:obtain_equipment("short_sword", 10)
    game:obtain_equipment("heaven_sword", 1)
end


function on_begin(game)
    -- Enemies
    game:generate_unit("Bandit", 12, Enum.force.enemy, {9, 6})
end


function on_victory(game)
end


function on_defeat(game)
end


function end_condition(game)
    if game:get_num_owns_alive() == 0 then
        return Enum.status.defeat
    end
    if game:get_num_enemies_alive() == 0 then
        return Enum.status.victory
    end
    return Enum.status.undecided
end


function main(game)
    game:set_on_deploy(on_deploy)
    game:set_on_begin(on_begin)
    game:set_on_victory(on_victory)
    game:set_on_defeat(on_defeat)
    game:set_end_condition(end_condition)
end
