#include <catch2/catch_test_macros.hpp>

#include <methan/core/configuration.hpp>
#ifndef METHAN_DEBUG
#define METHAN_DEBUG
#endif
#include <methan/utility/crc32.hpp>

TEST_CASE("StringIdentifier correctly distinguish non-matching elements", "[hash]") {
    REQUIRE(METHAN_IDENTIFIER("HCE79ciGRNv_VCUmX4Eg9pF9spA") != METHAN_IDENTIFIER("Vxgbr-xxOHTr77RBEdihPhIgyyc"));
    REQUIRE(METHAN_IDENTIFIER("r0tU1Y1t8i_KvSGvfiPer_tznEA") != METHAN_IDENTIFIER("0lWukSXfNK2czaBEO5jK33D64Z0"));
    REQUIRE(METHAN_IDENTIFIER("ebAwd6TOiaQcDkUquMti4uZPHeA") != METHAN_IDENTIFIER("hsKbIxwvIsM1anvbiePNiCwJfmA"));
    REQUIRE(METHAN_IDENTIFIER("KjN927umQOCgsRlc9N7RfG1SG_M") != METHAN_IDENTIFIER("0ymTUqQFMcor4mOZIb7MpA_B9zE"));
    REQUIRE(METHAN_IDENTIFIER("C_39zpv05yeVryta-qu-HJBzXW0") != METHAN_IDENTIFIER("MRsNkFlYJhRqMHR4Axx1Nbx7710"));
    REQUIRE(METHAN_IDENTIFIER("2Lc-bYgPudhMVjQXiQf9NBvpzHk") != METHAN_IDENTIFIER("I4VXAIr1WAuMlihWjRxsOfYGci8"));
    REQUIRE(METHAN_IDENTIFIER("iLgSkiDMcnTZTf1ahfw1TXvFmws") != METHAN_IDENTIFIER("15T4dkpTY9-Pzvj2eAzNk6kmrHI"));
    REQUIRE(METHAN_IDENTIFIER("n4Ga0eMKhkCVZIKbIi84QcAYPfg") != METHAN_IDENTIFIER("QnMXpaIMPcq9Di2WUWJnZB37TQg"));
    REQUIRE(METHAN_IDENTIFIER("qXiVg2j3ODbXFQqXeCPHe6NIzPM") != METHAN_IDENTIFIER("aYml3cdfm7jc3eU4AX_foYDJt70"));
    REQUIRE(METHAN_IDENTIFIER("-9m21k2XHBJZp_CARVDF5noPMIE") != METHAN_IDENTIFIER("XTN2CGI88oiLaqP3AgEQUPk8mlM"));
    REQUIRE(METHAN_IDENTIFIER("UmfGda1JevcOgO03TF_8IC51XS8") != METHAN_IDENTIFIER("mQcsSDjZpkz_OJSWmS5RpK1oOLM"));
    REQUIRE(METHAN_IDENTIFIER("5HhiZP6oDTnThw2Kbza_stZVIyA") != METHAN_IDENTIFIER("LXtPLt4E_fDQtIJgWLsoD2tB5hg"));
    REQUIRE(METHAN_IDENTIFIER("p7PjcVvzf2xEFVl-lbhKPWcjUyM") != METHAN_IDENTIFIER("C2Y4lBXNHiAgpWOipuG9aDdWkHE"));
    REQUIRE(METHAN_IDENTIFIER("6uZ8rlqs4mwEPnMobFKJtN2IUMs") != METHAN_IDENTIFIER("z941fgAW_yJfUxqKqBkbNJy5W9Y"));
    REQUIRE(METHAN_IDENTIFIER("t0PzThnRmLhHAlEQNQZKXKX0HGE") != METHAN_IDENTIFIER("j9txIMW8HQovLs_A7Eifa5bo8hQ"));
    REQUIRE(METHAN_IDENTIFIER("w7pEC4ME1EraznpjSL4LkRhbIjM") != METHAN_IDENTIFIER("HmdsKLDXkwRBWyWAqnCQsRN_Imo"));
    REQUIRE(METHAN_IDENTIFIER("HBNECxXioCY_EYNT8fEJbpK-N5U") != METHAN_IDENTIFIER("mGSayMD7QMMlFAN8Iqwg9knvj64"));
    REQUIRE(METHAN_IDENTIFIER("oSkxiVdHiBQVI5y68XGJjJn5Iao") != METHAN_IDENTIFIER("iPordoqiebkwyr7GK3EVl02M1Yc"));
    REQUIRE(METHAN_IDENTIFIER("ITI16fra9TrYtKJ72xyu-0s3Zlo") != METHAN_IDENTIFIER("Flt3wP9sig3GXrd8ljCKRSqXlYM"));
    REQUIRE(METHAN_IDENTIFIER("uiqetvd67Adtl6nprJy_MaKuS-k") != METHAN_IDENTIFIER("zUOhyktggrc6xiyXNEEitsd7K7Y"));
    REQUIRE(METHAN_IDENTIFIER("x8rioNhcx3WP9Boxq4yg_NwU_Zw") != METHAN_IDENTIFIER("G-4T9u3yE2r-QhLv9P4994_LbD8"));
    REQUIRE(METHAN_IDENTIFIER("TqPUMNRyHLbphducUskO3Md4yOY") != METHAN_IDENTIFIER("ST1j9QFo3joK5wgwsLiOcJpFANE"));
    REQUIRE(METHAN_IDENTIFIER("y-A59x6c88qo0yVeTkHqiYqJCeg") != METHAN_IDENTIFIER("l4H_6vj_abxIM1zdqXcMdqfrrE8"));
    REQUIRE(METHAN_IDENTIFIER("smeKn_-eWtfFykqaYPW4W5MJ7B0") != METHAN_IDENTIFIER("QMjSRvzJW72PoLuUezSsGWhNXeg"));
    REQUIRE(METHAN_IDENTIFIER("YRfFiCW94gr1FiGn_1icq1ZE6eM") != METHAN_IDENTIFIER("kMnj3GTOmHSuJXs0067bO6cBvO4"));
    REQUIRE(METHAN_IDENTIFIER("luufcqagkinRQ17g7tIycceVMvQ") != METHAN_IDENTIFIER("GS2SXZLdOPN24sAF1yTHZqtqH9Q"));
    REQUIRE(METHAN_IDENTIFIER("QnMznB-2_rDwSvvrTGu3x1ZKYfE") != METHAN_IDENTIFIER("4YD9DVDn37yYJVKOIldZTydXVKA"));
    REQUIRE(METHAN_IDENTIFIER("-i_Rdh2jj8WWcSYdMqbFeYCVm18") != METHAN_IDENTIFIER("Xrsol2TOMP5UDw6D-0pIkYLoRHo"));
    REQUIRE(METHAN_IDENTIFIER("FvPnxxCcbrCLMcCZeiz71qDlf8Y") != METHAN_IDENTIFIER("Or6tvq5lrVseZl_a32enLiYxgFY"));
    REQUIRE(METHAN_IDENTIFIER("tlMo71VDn7fQGrLvf8wRl-cJeVA") != METHAN_IDENTIFIER("nwThwsSnH_fAhUXZgZoL2eQnnk0"));
    REQUIRE(METHAN_IDENTIFIER("DwT8tMmK0H3HjrkBsbcTjBYq_eM") != METHAN_IDENTIFIER("9Pigarhae5mjglMN9LIy2vw-ZTY"));
    REQUIRE(METHAN_IDENTIFIER("QQhupeycPG02s8G_y_mJkcyyrlc") != METHAN_IDENTIFIER("2FyPKk8PrNulmNQ-6QguveACsDg"));
    REQUIRE(METHAN_IDENTIFIER("_i-FqNFlWYY3VKhiISQysZ2iiQw") != METHAN_IDENTIFIER("lz5pDZvlM0DQUTK_b-ItRdRa1Gc"));
    REQUIRE(METHAN_IDENTIFIER("S46j_fdREcHl1vYI78vxky1yrTc") != METHAN_IDENTIFIER("4PJLN2pgjix5tzwUVEVQuZ3EKtg"));
    REQUIRE(METHAN_IDENTIFIER("seE0gWrvOEb7Rx1RmuRGNacE9pg") != METHAN_IDENTIFIER("N_T-4fVhBOv3cKHGWPZgmqR3Cfk"));
    REQUIRE(METHAN_IDENTIFIER("W-yyQM2Bxa31SRhs34zslTiMIXE") != METHAN_IDENTIFIER("ehPD8gMUH53ynN8w-8mz4hsr6uc"));
    REQUIRE(METHAN_IDENTIFIER("rruaaL4rXRvz-Huwcb17o2txdCA") != METHAN_IDENTIFIER("kdfvkjdtdwau29LucGEy1aGerPk"));
    REQUIRE(METHAN_IDENTIFIER("q-B9JXAgJphWRuiOw-4ap2_N5B0") != METHAN_IDENTIFIER("emfpR5Nc5GKjJoBeNul3iusNDhY"));
    REQUIRE(METHAN_IDENTIFIER("DoiP7MZHvOr1Q5eN1HogRw5GuFw") != METHAN_IDENTIFIER("nI6NA4LIEL3-gqNVtji97oiBVAY"));
    REQUIRE(METHAN_IDENTIFIER("5f7hXeCXm8TMmeLi9q-lIK6Mtto") != METHAN_IDENTIFIER("EW8p7PNJceopPB8Tb3kW5KmsySQ"));
    REQUIRE(METHAN_IDENTIFIER("R7aNiu1Gr7yMxWoD2orenj2aANU") != METHAN_IDENTIFIER("Y3-CwkHA39VBtNGnGMpjn2D_ifw"));
    REQUIRE(METHAN_IDENTIFIER("0zhLj8qJ7eDUwrUPD2B8SoQlIXg") != METHAN_IDENTIFIER("WrQDqd04gjsiYuwi_foyrpB7y3w"));
    REQUIRE(METHAN_IDENTIFIER("P3JjDJh-iC5LUwP32Y2kpzF6E00") != METHAN_IDENTIFIER("dR8A1d3BRCY4R1-8Jun-XGLGeZA"));
    REQUIRE(METHAN_IDENTIFIER("7NwS0O6MTeejpGcfUA7F8Ixdsio") != METHAN_IDENTIFIER("w-oW4YiERhaBq_sDEnl5JgibBV0"));
    REQUIRE(METHAN_IDENTIFIER("lPhliAJUWP2wiitZWyi1TBxSUzY") != METHAN_IDENTIFIER("rxCZVch1PMlkcUQsY5hxVQEgxKY"));
    REQUIRE(METHAN_IDENTIFIER("EI8_xZv8MJuutTm790tTp6BGxdY") != METHAN_IDENTIFIER("NEiWs7XYj7qf0mfrznr68t7hEPk"));
    REQUIRE(METHAN_IDENTIFIER("UU3ZFJZyMixwcx5QEkwvgJG6ha8") != METHAN_IDENTIFIER("HcR7HO3H_y5iI9-TCIDWDWY6ryE"));
    REQUIRE(METHAN_IDENTIFIER("ac1IlRD6NgKi4x18K5Hn6P-HH9k") != METHAN_IDENTIFIER("ZGiPSI1hu1eA4ojqsp6nBZwiSks"));
    REQUIRE(METHAN_IDENTIFIER("SNT3y42jtBAHsVknQrmn9PLMhJM") != METHAN_IDENTIFIER("7xF16_w8PQEdw1tYQaEbPat7n50"));
    REQUIRE(METHAN_IDENTIFIER("1sSp2ymRqkcb5gglD-d_iHhZpgo") != METHAN_IDENTIFIER("buHKIIY2gn-0k3H88ydpgSPVluI"));
    REQUIRE(METHAN_IDENTIFIER("4MV_WmvYaidSJt9HELcK_67skwQ") != METHAN_IDENTIFIER("nv9-iYhmxmrJxIW7wGRmsT-p2m0"));
    REQUIRE(METHAN_IDENTIFIER("xTQKVtg0YSN2Q7fKA0TOMgrArcU") != METHAN_IDENTIFIER("Gr5WMr5qheqe0S9jRNVPQdT311M"));
    REQUIRE(METHAN_IDENTIFIER("n8KCQMMyW9LQa9w3vAk795usViI") != METHAN_IDENTIFIER("Sff86l15PDvCqRMOgwq9WcJyh0w"));
    REQUIRE(METHAN_IDENTIFIER("-rjzOBGhFXka-2MNMGENHcPmcm0") != METHAN_IDENTIFIER("B-YSc_6dIAT8a_fK5A3_DiY_Jfc"));
    REQUIRE(METHAN_IDENTIFIER("inxQEJaSNET0vNmZmDyj1I3gc1U") != METHAN_IDENTIFIER("NRtGLIwhxQv_uSi2Khze_OPGUHA"));
    REQUIRE(METHAN_IDENTIFIER("rT9fqIxa2ElYSxVKY9p4-7aR5U0") != METHAN_IDENTIFIER("_1-SEdR8bgs0wSrXTTH_oivwpfs"));
    REQUIRE(METHAN_IDENTIFIER("IRW3GrK0lxrVVg8YimAZBBMB6dE") != METHAN_IDENTIFIER("HHUhDvoO7MbRN_Cn9ZZfcztMeeg"));
    REQUIRE(METHAN_IDENTIFIER("lMkqYq1trqa02W_7Q9E_T03f0UE") != METHAN_IDENTIFIER("4zz-LQtZ1huUYDsaSgTCLKt3iZg"));
    REQUIRE(METHAN_IDENTIFIER("sZkKpOS03xKFEsoewtUnBUzcMeA") != METHAN_IDENTIFIER("b8g62pywcaVRr5uOYrRIXDyv0mY"));
    REQUIRE(METHAN_IDENTIFIER("TN8010e44MTAjgrHbLzscsmUAvE") != METHAN_IDENTIFIER("6A7NRzW23FXmzAHGNQcYfbofUGQ"));
    REQUIRE(METHAN_IDENTIFIER("OLDFsTNwlPYWCV7C1pt3feE5U0Y") != METHAN_IDENTIFIER("fHAW9gWXxnw8hnFcO4kNoEYLW1E"));
    REQUIRE(METHAN_IDENTIFIER("hJb4ui3LBGWmvy8-obAlbBkBjyE") != METHAN_IDENTIFIER("DuQnNFXDlzk7deUfX3dt_qjDgVM"));
    REQUIRE(METHAN_IDENTIFIER("jh2-skoeEpTlh5JDyGzySUhKmUo") != METHAN_IDENTIFIER("QLc4V9f908H8aH9mfnAEzDlcvoo"));
    REQUIRE(METHAN_IDENTIFIER("nUYZEhsn6tjxUEYCy14K8fy6p6g") != METHAN_IDENTIFIER("NHvFJjNcNjGgjRidlnl5oGJm3HI"));
    REQUIRE(METHAN_IDENTIFIER("cFSoUuj38-up6JpHTtwBXQlDbkE") != METHAN_IDENTIFIER("c2i9N_xPdZYuaHnTQCXGynLvPWc"));
    REQUIRE(METHAN_IDENTIFIER("EwDNwjeCWiEttHG_sKEMSZyEI0o") != METHAN_IDENTIFIER("GGTaCdf88gaT9W8L8RUj7g5iH-A"));
    REQUIRE(METHAN_IDENTIFIER("FduiPieoPayfSl3OO2WIcgxhOYA") != METHAN_IDENTIFIER("ZvT7e9aZG1CY-BhC7pb_qCC0olI"));
    REQUIRE(METHAN_IDENTIFIER("r_rGMeWzlgscjZzbAdpjagLjG-s") != METHAN_IDENTIFIER("Jyv2O9U7s0DBL9db7huxP7uu5eA"));
    REQUIRE(METHAN_IDENTIFIER("xIGnQB3ZXMMNphQCAiXHyZJJ2Hs") != METHAN_IDENTIFIER("lH0ha0Iv8f0Vu3T-STwUD3jUMLg"));
    REQUIRE(METHAN_IDENTIFIER("ZxwZ288entCOYz06OfcUXJvl8ZY") != METHAN_IDENTIFIER("fkd_i8Mu3JLdtIjZn39od2T04e0"));
    REQUIRE(METHAN_IDENTIFIER("nugL6AuFwEkRImniJbJRy_I47w4") != METHAN_IDENTIFIER("hqAXpbUemJRWLNCCOkdrPHW7Gh8"));
    REQUIRE(METHAN_IDENTIFIER("pCtoPl-ciSY-0owDlOteRAEYaxI") != METHAN_IDENTIFIER("FEpzuY2ZLUaXDC9dJFhaKQy3M2A"));
    REQUIRE(METHAN_IDENTIFIER("7vsBc37bAKiR--evfnVNtKGAjqM") != METHAN_IDENTIFIER("1q-3XjidkH7gjIlPj1tntw_FkgQ"));
    REQUIRE(METHAN_IDENTIFIER("jDzSQaVJdXTJs1JqCGyBhQL4shI") != METHAN_IDENTIFIER("jFPolXcjVEHJRD3rSsyxl4h4uEo"));
    REQUIRE(METHAN_IDENTIFIER("IwvOC6xjBbTS4qapfvD4QaW8_NY") != METHAN_IDENTIFIER("SgGCpnQN0RiA32wsMIxv3Tfsx9A"));
    REQUIRE(METHAN_IDENTIFIER("goy_b1LONTZE-3y1ETCsIogkpyE") != METHAN_IDENTIFIER("qRpXx8rXHFUGezZL9u2TSilaMHM"));
    REQUIRE(METHAN_IDENTIFIER("BF4eXgOjjPP3JVniqGt6-2z_W2I") != METHAN_IDENTIFIER("izJzCtUTINzo8ETGhcc3eS0ITX4"));
    REQUIRE(METHAN_IDENTIFIER("5nWSBWGlbtfXWVX8Oher8GXSBxI") != METHAN_IDENTIFIER("_NLXl2ZgBoY2iSkGLIftxN394Lc"));
    REQUIRE(METHAN_IDENTIFIER("TgBAxCLHk1Hx2Wm0pkKhZeajlFQ") != METHAN_IDENTIFIER("fpXqZ7NjTeiJmOBlK8XMD5g674c"));
    REQUIRE(METHAN_IDENTIFIER("045bzy9U18Yx_dVAGtyu5D1ChXc") != METHAN_IDENTIFIER("3DE8AsHg1rXHXCJWBMEjwW07SLU"));
    REQUIRE(METHAN_IDENTIFIER("sgS6oBenBwesS0P1ykOKDqQZIAM") != METHAN_IDENTIFIER("7n4-wSP2ySD02e7OAbYpw85xOQw"));
    REQUIRE(METHAN_IDENTIFIER("K0LpjJAN9lY9Yy2kid64NWySS2w") != METHAN_IDENTIFIER("o8P88-wLY_UuC00qT0FPjV5KdC8"));
    REQUIRE(METHAN_IDENTIFIER("jvm-m9tjQwGh_sDNyRjoe_liPA0") != METHAN_IDENTIFIER("8XHmE8tyjT7UJZ4FHcY9sdP5JTY"));
    REQUIRE(METHAN_IDENTIFIER("FBjAEwbRGAfyiwHnaTWgRqrk9Ds") != METHAN_IDENTIFIER("WJj3jogkav-JCGOvEONaJc-Cse4"));
    REQUIRE(METHAN_IDENTIFIER("fpVajx_8tjTbj3E9ARXo0_xwa3M") != METHAN_IDENTIFIER("zRj_SsHn-9opdnOTW1WsGzAYQgk"));
    REQUIRE(METHAN_IDENTIFIER("XJR3yxljTGLi1ipo5b5BsGXs9-0") != METHAN_IDENTIFIER("iY85BTIq4DGlU9KMQQvc3bhZfoM"));
    REQUIRE(METHAN_IDENTIFIER("bZCV_LPHbZiHg-ALiJQc17MaeTs") != METHAN_IDENTIFIER("tPYIjPdM4rp4J0wx5_KzTsPpxUk"));
    REQUIRE(METHAN_IDENTIFIER("WIAQF0csiH1CqRdVEXaBmJvnCEQ") != METHAN_IDENTIFIER("fEXnd7vluowO9-BQaLrH5_Y2QNs"));
    REQUIRE(METHAN_IDENTIFIER("O42QP8oNYmUtvok3URW4wNBLyXI") != METHAN_IDENTIFIER("Z9zKjTEaP9RY4n12uIPYs3HD4Ss"));
    REQUIRE(METHAN_IDENTIFIER("64thwl5NogxVaAihKBalCMNy1dU") != METHAN_IDENTIFIER("Y-XFu4cxL9TayKfkfS6tM8jKY9M"));
    REQUIRE(METHAN_IDENTIFIER("LObU1Y3HyHJwa6cGzCs5zXeE5Oc") != METHAN_IDENTIFIER("EFdq8ePgu1Cn01Ua7oRgDvQF6xM"));
    REQUIRE(METHAN_IDENTIFIER("6GCFu73R8QinpA7rl2rmEeDzEYw") != METHAN_IDENTIFIER("mteqT9HpGV6cNuD91eSRbjJJZ_w"));
    REQUIRE(METHAN_IDENTIFIER("sSP2p1Kf5mWsWEdz6255xOu3-I8") != METHAN_IDENTIFIER("iPHVNljobP3hjwWTsoT0A9zJ6Zw"));
    REQUIRE(METHAN_IDENTIFIER("jaecNm_SsLedPQImRWAsgIW1yUI") != METHAN_IDENTIFIER("mwC4ZPMWbiN4QlP5ivHDcdKvTX4"));
    REQUIRE(METHAN_IDENTIFIER("85YcClyuzzRh_-M51NhaKXbnjb8") != METHAN_IDENTIFIER("pmZh8QSv2XTZlWnB7m8nK65xItA"));
    REQUIRE(METHAN_IDENTIFIER("Aw37TVcJv0kBSY2srFaQckmUwKo") != METHAN_IDENTIFIER("Bd8uAxBmOgKsqQd1ckHt89NftBo"));
    REQUIRE(METHAN_IDENTIFIER("4jRa3BagHRhFmJSEb2hfCJEdr5U") != METHAN_IDENTIFIER("6twQclZOgLuuo85MgoPpcwn-dlU"));
    REQUIRE(METHAN_IDENTIFIER("EWDMZv3DnkuzNfjzJfx_kQR6WtY") != METHAN_IDENTIFIER("asw32n6ULPOfnisn9td2XZ87EUQ"));
    REQUIRE(METHAN_IDENTIFIER("7MVFYZiDjWhyF0ZlkkslCWBfRRI") != METHAN_IDENTIFIER("8ptRgX14Nc0JWc14lAT0eolfkPQ"));
    REQUIRE(METHAN_IDENTIFIER("9NIPsXL5Kiq9GH5sokdJbsdBalE") != METHAN_IDENTIFIER("RoqodzJMZUwSqIZmG3htxHG3w08"));    
}

TEST_CASE("StringIdentifier is correctly equal onto the same input", "[hash]") {
    REQUIRE(METHAN_IDENTIFIER("HCE79ciGRNv_VCUmX4Eg9pF9spA") == METHAN_IDENTIFIER("HCE79ciGRNv_VCUmX4Eg9pF9spA"));
    REQUIRE(METHAN_IDENTIFIER("HELLO WORLD") == METHAN_IDENTIFIER("HELLO WORLD"));
    REQUIRE(METHAN_IDENTIFIER("This Is Not What You Might Think But Hey") == METHAN_IDENTIFIER("This Is Not What You Might Think But Hey"));
}
